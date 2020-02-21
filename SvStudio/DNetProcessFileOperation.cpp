#include "DNetProcessFileOperation.h"
#include "SvNet/SvNetHandlerFile.h"


DNetProcessFileOperation::DNetProcessFileOperation(qint8 oper, const QString src, const QString dst, const QString descr, SvNetChannel *channel, QWidget *parent) :
  DNetProcess( descr, channel, parent ),
  mSrc(src),
  mDst(dst),
  mOper(oper)
  {

  }

void DNetProcessFileOperation::receivedBlock(SvNetChannel *ch, int cmd, const QByteArray block)
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

void DNetProcessFileOperation::start()
  {
  SvNetFileOper oper(mSrc,mDst);
  emit sendBlock( mChannel, mOper, oper.buildBlock() );
  }
