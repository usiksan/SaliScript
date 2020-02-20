#include "SvNetHandlerMirror.h"
#include "SvNetMirror.h"


SvNetHandlerMirror::SvNetHandlerMirror()
  {

  }

void SvNetHandlerMirror::mirrorChanged(int id, SvMirrorPtr mirrorPtr)
  {
  Q_UNUSED(id)
  mMirror = mirrorPtr;
  connect( mMirror, &SvMirror::log, this, [this] ( const QString msg ) {
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
      for( int i = 0; i < count; i++ )
        state.mMemory.append( mMirror->memoryGet( i ) );

      //Send answer
      emit sendBlock( ch, SVC_MIRROR_STATUS, state.buildBlock() );
      }
    }
  }





