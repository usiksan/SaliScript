#include "DNetProcessFileSend.h"
#include "SvNet/SvNetHandlerFile.h"


DNetProcessFileSend::DNetProcessFileSend(const QString remoteFileName, const QByteArray fileContent, const QString descr, SvNetChannel *channel, QWidget *parent) :
  DNetProcess( descr, channel, parent ),
  mRemoteFileName(remoteFileName),
  mFileContent(fileContent)
  {

  }




void DNetProcessFileSend::receivedBlock(SvNetChannel *ch, int cmd, const QByteArray block)
  {
  if( mChannel == ch && cmd == SVC_FILE_ANSWER ) {
    SvNetAnswer answer(block);
    if( answer.mAnswerCode ) {
      mError->setText( answer.mMessage );
      mClose->setText( tr("Close error") );
      }
    else done(1);
    }
  }




void DNetProcessFileSend::start()
  {
  SvNetFile netFile( mRemoteFileName );
  netFile.mContent = mFileContent;
  emit sendBlock( mChannel, SVC_FILE, netFile.buildBlock() );
  }


