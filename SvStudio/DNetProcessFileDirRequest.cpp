#include "DNetProcessFileDirRequest.h"

DNetProcessFileDirRequest::DNetProcessFileDirRequest(const QString path, const QString descr, SvNetChannel *channel, QWidget *parent) :
  DNetProcess( descr, channel, parent ),
  mDirInfo( path )
  {

  }

void DNetProcessFileDirRequest::receivedBlock(SvNetChannel *ch, int cmd, const QByteArray block)
  {
  if( mChannel == ch ) {
    if( cmd == SVC_FILE_DIR ) {
      SvNetDirInfo info(block);
      mDirInfo = info;
      done(1);
      }
    else if( cmd == SVC_FILE_ANSWER ) {
      SvNetFileAnswer answer(block);
      mError->setText( answer.mMsg );
      mClose->setText( tr("Close error") );
      }
    }
  }

void DNetProcessFileDirRequest::start()
  {
  emit sendBlock( mChannel, SVC_FILE_REQUEST, mDirInfo.buildBlock() );
  }
