/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvMirrorRemote.h"
#include "SvNet/SvNetMirror.h"

#include <QHostAddress>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHashData>
#include <QCryptographicHash>
#include <QDataStream>

enum {
  FSM_IDLE,
  FSM_WAIT_RECONNECT,
  FSM_CONNECT,
  FSM_WAIT_CONNECTION,
  FSM_SEND_CONTROL,
  FSM_WAIT_STATUS,
  FSM_WAIT_PERIOD,
  FSM_SEND_PROGRAMM,
  FSM_WAIT_PROGRAMM
  };



SvMirrorRemote::SvMirrorRemote() :
  SvMirrorExtern(),
  mChannel(nullptr),
  mFsmCurrent(FSM_IDLE)
  {
  }



SvMirrorRemote::~SvMirrorRemote()
  {

  }

QString SvMirrorRemote::controllerType() const
  {
  return tr("Remote ip %1 -> %2").arg(mIp).arg(mControllerType);
  }



void SvMirrorRemote::linkTo(const QString ipOrUsb, int port, const QString controllerName, const QString controllerPassw)
  {
  Q_UNUSED(controllerName)
  Q_UNUSED(controllerPassw)
  mIp   = ipOrUsb;
  mPort = port;
  reconnect();
  }




void SvMirrorRemote::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause, bool flash)
  {
  //Assign new programm, apply run or pause and set flag that programm need to flash
  mProgramm   = prog;
  mRunOrPause = runOrPause;
  mNeedFlash  = flash;
  //Locker to value queue write
  QMutexLocker locker(&mWriteMutex);
  mWriteQueue.clear();
  mWriteValues.clear();
  //Clear debug
  clearDebug();
  emit transferProcess( false, tr("Transfer programm...") );
  }




void SvMirrorRemote::processing(int tickOffset)
  {
  switch( mFsmCurrent ) {
    case FSM_IDLE :
      //Do nothing
      break;



    case FSM_WAIT_RECONNECT :
      mTimeOut -= tickOffset;
      if( mTimeOut < 0 )
        mFsmCurrent = FSM_CONNECT;
      break;



    case FSM_CONNECT :
      //Need connection
      if( mIp.isEmpty() || mPort == 0 )
        mFsmCurrent = FSM_IDLE;
      else if( mChannel->getSocket()->state() == QAbstractSocket::UnconnectedState ) {

        //Try to connect with current ip and port settings
        mChannel->getSocket()->connectToHost( QHostAddress(mIp), mPort );

        //Connection timeout
        mTimeOut = 1000;
        mFsmCurrent = FSM_WAIT_CONNECTION;
        }
      break;


    case FSM_WAIT_CONNECTION :
      if( mChannel->getSocket()->state() == QAbstractSocket::ConnectedState ) {
        //Connection established
        mFsmCurrent = FSM_SEND_PROGRAMM;
        }
      else if( --mTimeOut < 0 ) {
        //Connection failure
        reconnect();
        }
      break;


    case FSM_SEND_CONTROL : {
      SvNetMirrorControl control;
      if( mWriteQueue.count() ) {
        //There are values for write
        //Блокиратор сдвоенного доступа к очереди
        QMutexLocker locker(&mWriteMutex);
        control.mWriteQueue = mWriteQueue;
        control.mWriteValues = mWriteValues;
        mWriteQueue.clear();
        mWriteValues.clear();
        }
      control.mVpuDebug = mVpuDebug;
      //Clear debug
      clearDebug();

      mChannel->sendBlock( mChannel, SVC_MIRROR_CONTROL, control.buildBlock() );
      mFsmCurrent = FSM_WAIT_STATUS;
      mTimeOut = 3000;
      }
      break;


    case FSM_WAIT_PROGRAMM :
    case FSM_WAIT_STATUS :
      mTimeOut -= tickOffset;
      if( mTimeOut < 0 )
        reconnect();
      break;



    case FSM_WAIT_PERIOD :
      mTimeOut -= tickOffset;
      if( mTimeOut < 0 )
        mFsmCurrent = FSM_SEND_CONTROL;
      break;




    case FSM_SEND_PROGRAMM :
      if( mNeedFlash ) {
        //Подготовить и отправить блок с программой
        mChannel->sendBlock( mChannel, SVC_MIRROR_PROGRAMM, SvNetMirrorProgramm::buildBlock( mRunOrPause, mProgramm->toArray() ) );
        //Шаг ожидания завершения загрузки программы
        mFsmCurrent = FSM_WAIT_PROGRAMM;
        mTimeOut = 3000;
        }
      else
        mFsmCurrent = FSM_SEND_CONTROL;
      break;
    }
  }





void SvMirrorRemote::init()
  {
  mChannel = new SvNetChannel( new QTcpSocket() );
  connect( mChannel->getSocket(), &QTcpSocket::disconnected, this, [this] {
    mControllerType.clear();
    mProgrammName.clear();
    emit linkChanged( false, controllerType(), programmName() );
    //Попытаться подключиться через 1 сек
    reconnect();
    });
  connect( mChannel, &SvNetChannel::receivedBlock, this, &SvMirrorRemote::receivedBlock );
  }





void SvMirrorRemote::receivedBlock(SvNetChannel *ch, int cmd, const QByteArray block)
  {
  Q_UNUSED(ch)
  if( cmd == SVC_MIRROR_STATUS ) {
    SvNetMirrorStatus status( block );
    if( mControllerType != status.mControllerType || mProgrammName != status.mProgrammName ) {
      mControllerType = status.mControllerType;
      mProgrammName   = status.mProgrammName;
      emit linkChanged( true, controllerType(), programmName() );
      }
    mVpuState = status.mVpuState;
    mMemory   = status.mMemory;
    for( auto msg : status.mLog )
      emit log( msg );
    emit memoryChanged( this );
    if( mVpuDebug.count() < mVpuState.count() ) {
      //Дополнить количество отладочных ячеек до количества процессов
      mVpuDebug.resize( mVpuState.count() );
      clearDebug();
      }
    mFsmCurrent = FSM_SEND_PROGRAMM;
    }
  else if( cmd == SV_NET_CHANNEL_ANSWER_CMD ) {
    SvNetAnswer answer( block );
    if( answer.mSrcCmd == SVC_MIRROR_PROGRAMM ) {
      //Acknowledge to programm load
      mTimeOut = 200;
      mFsmCurrent = FSM_WAIT_PERIOD;
      mNeedFlash = false;
      emit transferProcess( true, QString{} );
      }
    }
  }





void SvMirrorRemote::reconnect()
  {
  mChannel->getSocket()->disconnectFromHost();
  mTimeOut = 1000;
  mFsmCurrent = FSM_WAIT_RECONNECT;
  }

