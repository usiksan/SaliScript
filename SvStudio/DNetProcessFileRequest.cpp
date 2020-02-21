#include "DNetProcessFileRequest.h"
#include "SvNet/SvNetHandlerFile.h"

DNetProcessFileRequest::DNetProcessFileRequest(const QString remoteFileName, const QString descr, SvNetChannel *channel, QWidget *parent) :
  DNetProcess( descr, channel, parent ),
  mRemoteFileName(remoteFileName)
  {

  }

void DNetProcessFileRequest::receivedBlock(SvNetChannel *ch, int cmd, const QByteArray block)
  {
  if( mChannel == ch ) {
    if( cmd == SVC_FILE ) {
      SvNetFile file(block);
      mFileContent = file.mContent;
      done(1);
      }
    else if( cmd == SVC_FILE_ANSWER ) {
      SvNetAnswer answer(block);
      mError->setText( answer.mMessage );
      mClose->setText( tr("Close error") );
      }
    }
  }




void DNetProcessFileRequest::start()
  {
  SvNetFile netFile( mRemoteFileName );
  emit sendBlock( mChannel, SVC_FILE_REQUEST, netFile.buildBlock() );
  }
