#include "SvNetHandlerMirror.h"
#include "SvNetMirror.h"


SvNetHandlerMirror::SvNetHandlerMirror()
  {

  }

void SvNetHandlerMirror::mirrorChanged(int id, SvMirrorPtr mirrorPtr)
  {
  Q_UNUSED(id)
  mMirror = mirrorPtr;
  connect( mMirror.data(), &SvMirror::log, this, [this] ( const QString msg ) {
    mLogList.append(msg);
    });
  }


void SvNetHandlerMirror::receivedBlock(SvNetChannel *ch, qint8 cmd, QByteArray block)
  {
  if( cmd == SVC_MIRROR_CONTROL ) {
    SvNetMirrorControl control(block);

    if( mMirror ) {
      //Apply debug command
      for( int i = 0; i < control.mVpuDebug.count(); i++ )
        if( control.mVpuDebug.at(i).mCommand )
          mMirror->debug( i, control.mVpuDebug.at(i).mCommand, control.mVpuDebug.at(i).mParam1, control.mVpuDebug.at(i).mParam2 );

      //Apply write values
      for( auto addr : control.mWriteQueue )
        mMirror->memorySet( addr, control.mWriteValues.value(addr) );

      //Prepare answer with mirror state
      SvNetMirrorStatus state;
      state.mControllerType = mMirror->controllerType();
      state.mProgrammName   = mMirror->programmName();
      state.mVpuState       = mMirror->vpuVector();
      state.mLog            = mLogList;
      mLogList.clear();
      int count = mMirror->memoryGlobalCount();
      state.mMemory.append( 0 );
      for( int i = 1; i < count; i++ )
        state.mMemory.append( mMirror->memoryGet( i ) );

      //Send answer
      emit sendBlock( ch, SVC_MIRROR_STATUS, state.buildBlock() );
      }
    }
  else if( cmd == SVC_MIRROR_PROGRAMM ) {
    //Принята программа для загрузки
    if( mMirror ) {
      //Декодировать программу
      SvNetMirrorProgramm prg( block );
      SvProgrammPtr prog( new SvProgramm() );
      prog->fromArray( prg.mProgramm );

      //Установить новую программу
      mMirror->setProgrammFlashRun( prog, prg.mRunOrPause, true );

      //Отправить ответ
      emit sendBlock( ch, SV_NET_CHANNEL_ANSWER_CMD, SvNetAnswer( SVC_MIRROR_PROGRAMM, 0, QString{} ).buildBlock() );
      }
    else
      emit sendBlock( ch, SV_NET_CHANNEL_ANSWER_CMD, SvNetAnswer( SVC_MIRROR_PROGRAMM, 1, tr("No mirror") ).buildBlock() );
    }
  }





