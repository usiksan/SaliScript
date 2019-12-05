#if 0
#include "SvMirrorCom.h"


SvMirrorCom::SvMirrorCom(const QString portName) :
  SvMirrorExtern()
  {

  }

void SvMirrorCom::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause)
  {

  }




bool SvMirrorExtern::taskInfo(qint32 taskId, SvVmVpuState &destTaskInfo) const
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    destTaskInfo = mVpuState[taskId];
    return true;
    }
  return false;
  }







//Раздел памяти данных
int SvMirrorExtern::memoryGet(int index)
  {
  if( index > 0 && index < mMemorySize )
    return mMemory[index];
  return 0;
  }






void SvMirrorExtern::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause)
  {
  mReset = true;
  mRun = runOrPause;
  mProgramm = prog;
  }




void SvMirrorExtern::processing(int tickOffset)
  {
  mReceivTimeOut -= tickOffset;
  if( mReceivTimeOut < 0 ) {
    if( !mControllerInfo.mLink ) {
      //Not linked yet
      mReceivTimeOut = 1000;
      //Query controller version
      mBuf[0] = SVU_CMD_VERSION_GET;
      send( mBuf );
      }
    else {
      //Read time-out
      mControllerInfo.mLink = false;
      mReceivTimeOut = 1000;
      }
    }
  }




void SvMirrorExtern::onReceived(const unsigned char *buf)
  {
  //For receiv time out
  mReceivTimeOut = 100;

  //If reset flag then query reset
  if( mReset ) {
    queryReset();
    return;
    }

  switch( buf[0] ) {
    case SVU_CMD_VERSION :
      //Version received
      parseVersion( buf );

      //Get status
      queryState();
      break;

    case SVU_CMD_STATE :
      //State received
      parseState( buf );
      //If log received then retry state query until all log received
      if( buf[2] )
        //Query state for next log
        queryState();
      else
        queryMemory();
      break;

    case SVU_CMD_VARS_WRITE_OK :
      //Block of variables writed successfully, remove from queue
      queryMemory();
      break;

    case SVU_CMD_VARS_BLOCK :
      //Memory block received
      //Update variables
      parseVariables( buf );
      break;

    case SVU_CMD_VPU_STATE :
      //Task info received
      parseTask( buf );
      break;

    case SVU_CMD_DEBUG_OK :
      queryNextDebug();
      break;

    case SVU_CMD_VPU_RESET_OK :
      if( mFlash )
        queryFlashRead();
      else
        queryRestart();
      break;

    case SVU_CMD_VPU_RESTART_OK :
      queryMemory();
      break;

    case SVU_CMD_FLASH_BLOCK :
      //Start flash block received
      parseFlash(buf);
      break;

    case SVU_CMD_FLASH_ERASE_OK :
      queryNextFlash( 0 );
      break;

    case SVU_CMD_FLASH_WRITE_OK :
      parseFlashWriteOk( buf );
      break;
    }
  }




void SvMirrorExtern::parseVersion(const unsigned char *buf)
  {
  mLogLen = 0;
  //Version received
  mControllerInfo.mLink = true;
  //data [1-4]   version number
  mControllerInfo.mVersion = svIRead32( buf + 1 );
  //     [5-8]   programm memory size
  mControllerInfo.mProgrammMaxSize = svIRead32( buf + 5 );
  //     [9-10]  data memory size
  mControllerInfo.mVariableMaxCount = svIRead16( buf + 9 );
  //     [11]    VPU max count
  mControllerInfo.mVpuMax = svILimit( buf[11], 1, 256 );
  //     [12-31] controller type
  char str[21];
  memcpy( str, buf + 12, 20 );
  str[20] = 0;
  mControllerInfo.mType = QString::fromLatin1( str );

  QMutexLocker locker( &mVpuMutex );
  if( mVpuState )
    delete mVpuState;
  mVpuState = new SvVmVpuState[static_cast<unsigned>(mControllerInfo.mVpuMax)];
  memset( mVpuState, 0, sizeof(SvVmVpuState) * static_cast<unsigned>(mControllerInfo.mVpuMax) );

  if( mVpuDebug )
    delete mVpuDebug;
  mVpuDebug = new SvDebugTask[static_cast<unsigned>(mControllerInfo.mVpuMax)];
  memset( mVpuDebug, 0, sizeof(SvDebugTask) * static_cast<unsigned>(mControllerInfo.mVpuMax) );

  if( mMemory )
    delete mMemory;
  mMemory = new int[static_cast<unsigned>(mControllerInfo.mVariableMaxCount)];
  memset( mMemory, 0, sizeof(int) * static_cast<unsigned>(mControllerInfo.mVariableMaxCount) );
  emit controllerInfoChanged( this );
  }






void SvMirrorExtern::parseState(const unsigned char *buf)
  {
  //data [1]    - active VPU count
  mVpuCount = buf[1];
  //     [2]    - log byte count
  int logCount = buf[2];
  //     [3-63] - log contents
  if( logCount ) {
    for( int i = 0; i < logCount; i++ ) {
      if( buf[i + 3] == 0 || mLogLen > 1020 ) {
        mLog[mLogLen] = 0;
        emit log( QString::fromLatin1( mLog ) );
        mLogLen = 0;
        }
      else mLog[mLogLen++] = static_cast<char>( buf[i + 3] );
      }
    }
  }






void SvMirrorExtern::parseVariables(const unsigned char *buf)
  {
  int addr = svIRead16( buf + 2 ) & 0xffff;
  if( addr + buf[1] <= mControllerInfo.mVariableMaxCount ) {
    for( int i = 0; i < buf[1]; i++ )
      mMemory[addr++] = svIRead32( buf + 2 + 4 * i );

    addr += buf[1];
    if( addr < mProgramm->globalCount() ) {
      //Query next memory block
      int globalCount = svILimit( mProgramm->globalCount() - addr, 1, 15 );
      mBuf[0] = SVU_CMD_VARS_READ;
      //For variables time out
      mReceivTimeOut = 100;
      mBuf[1] = static_cast<unsigned char>(globalCount);
      svIWrite16( mBuf + 2, addr );
      send( mBuf );
      return;
      }

    //All memory readed
    emit memoryChanged();
    }

  if( mScanTasks )
    queryNextDebug();
  else
    queryState();
  }



void SvMirrorExtern::parseTask(const unsigned char *buf)
  {
  //data [1]     - VPU index
  int taskId = mBuf[1];
  if( taskId < mControllerInfo.mVpuMax ) {
    //     [2-5]   - ip
    mVpuState[taskId].mIp = svIRead32( buf + 2 );
    //     [6-9]   - sp
    mVpuState[taskId].mSp = svIRead32( buf + 6 );
    //     [10-13] - bp
    mVpuState[taskId].mBp = svIRead32( buf + 10 );
    //     [14-17] - tm
    mVpuState[taskId].mTm = svIRead32( buf + 14 );
    //     [18-21] - baseSp
    mVpuState[taskId].mBaseSp = svIRead32( buf + 18 );
    //     [22-25] - throw
    mVpuState[taskId].mThrow = svIRead32( buf + 22 );
    //     [26]    - run
    mVpuState[taskId].mDebugRun = buf[26];

    taskId++;
    if( taskId < mVpuCount ) {
      //Query next task
      mBuf[0] = SVU_CMD_VPU_STATE_GET;
      mBuf[1] = static_cast<unsigned char>(taskId);
      send( mBuf );
      return;
      }
    }
  queryState();
  }




void SvMirrorExtern::parseFlash(const unsigned char *buf)
  {
  //Compare with programm
  bool equal = true; //At begin we assume that flash equals
  for( int i = 0; i < SVVMH_HEADER_SIZE && equal; i++ )
    equal = buf[i] == mProgramm->getCode(i);
  if( equal )
    //Flash not need. Immediately restart
    queryRestart();
  else {
    //Need to flash
    mBuf[0] = SVU_CMD_FLASH_ERASE;
    send( mBuf );
    emit transferProcess( false, tr("Erase flash...") );
    mReceivTimeOut = 6000;
    }
  }




void SvMirrorExtern::parseFlashWriteOk(const unsigned char *buf)
  {
  int len = buf[1];
  int addr = svIRead32( buf + 2 );
  addr += len;
  queryNextFlash( addr );
  }






void SvMirrorExtern::queryMemory()
  {
  if( mWriteQueue.count() ) {
    //Memory write
    mBuf[0] = SVU_CMD_VARS_WRITE;
    //For variables time out
    mReceivTimeOut = 100;
    //Блокировщик доступа к очереди
    QMutexLocker locker( &mWriteMutex );

    //Сформировать пакет на установку
    int size = qMin( mWriteQueue.count(), 10 );
    mBuf[1] = SVU_CMD_VARS_WRITE;
    mBuf[2] = static_cast<unsigned char>( size );

    for( int i = 0; i < size; i++ ) {
      int adr = mWriteQueue.at(i);
      int val = mWriteValues.value(adr);
      svIWrite16( mBuf + 3 + i * 6, adr );
      svIWrite32( mBuf + 5 + i * 6, val );
      }

    //Remove sended variables
    for( int i = 0; i < size; i++ )
      mWriteValues.remove( mWriteQueue.takeFirst() );

    send( mBuf );
    }
  else {
    //Memory read
    int globalCount = svILimit( mProgramm->globalCount(), 1, 15 );
    mBuf[0] = SVU_CMD_VARS_READ;
    //For variables time out
    mReceivTimeOut = 100;
    mBuf[1] = static_cast<unsigned char>(globalCount);
    svIWrite16( mBuf + 2, 0 );
    send( mBuf );
    }
  }




void SvMirrorExtern::queryNextDebug()
  {
  for( int i = 0; i < mControllerInfo.mVpuMax; i++ )
    if( mVpuDebug[i].mCommand ) {
      //Execute debug command
      QMutexLocker locker( &mVpuMutex );

      mBuf[0] = SVU_CMD_DEBUG;
      mBuf[1] = static_cast<unsigned char>(i);
      mBuf[2] = static_cast<unsigned char>(mVpuDebug[i].mCommand);
      svIWrite32( mBuf + 3, mVpuDebug[i].mParam1 );
      svIWrite32( mBuf + 7, mVpuDebug[i].mParam2 );
      send( mBuf );
      mVpuDebug[i].mCommand = 0;
      return;
      }

  //Query next task
  mBuf[0] = SVU_CMD_VPU_STATE_GET;
  mBuf[1] = 0;
  send( mBuf );
  }




void SvMirrorExtern::queryNextFlash( int addr )
  {
  if( addr < mProgramm->codeCount() ) {
    int len = svILimit( mProgramm->codeCount() - addr, 1, 58 );
    mBuf[0] = SVU_CMD_FLASH_WRITE;
    mBuf[1] = static_cast<unsigned char>(len);
    svIWrite32( mBuf + 2, addr );
    for( int i = 0; i < len; i++ )
      mBuf[i + 6] = static_cast<unsigned char>( mProgramm->getCode(addr + i) );
    send( mBuf );
    emit transferProcess( false, tr("Programming %1 from %2").arg(addr).arg(mProgramm->codeCount()) );
    }
  else queryRestart();
  }



void SvMirrorExtern::queryState()
  {
  //Command
  mBuf[0] = SVU_CMD_STATE_GET;
  //Send query
  send( mBuf );
  }



void SvMirrorExtern::queryReset()
  {
  mReset = false;
  //Command
  mBuf[0] = SVU_CMD_VPU_RESET;
  //Send query
  send( mBuf );
  }



void SvMirrorExtern::queryFlashRead()
  {
  mFlash = false;
  mBuf[0] = SVU_CMD_FLASH_READ;
  mBuf[1] = SVVMH_HEADER_SIZE;
  svIWrite32( mBuf + 2, 0 );
  send( mBuf );
  }




void SvMirrorExtern::queryRestart()
  {
  mBuf[0] = SVU_CMD_VPU_RESTART;
  mBuf[1] = static_cast<unsigned char>(mRun);
  send( mBuf );
  emit transferProcess( true, QString() );
  }





#endif
