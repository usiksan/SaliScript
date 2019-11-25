#include "SvNetChannelFile.h"


SvNetChannelFile::SvNetChannelFile()
  {

  }




void SvNetChannelFile::onReceivedBlock(SvNetChannel *ch, int cmd, QByteArray block)
  {
  switch(cmd) {
    case SVC_DIR_REQUEST :
      onDirListRequest( ch, block );
      break;
    }

  }






void SvNetChannelFile::setChannel(SvNetChannel *ch)
  {
  }




void SvNetChannelFile::onDirListRequest(SvNetChannel *ch, const QByteArray bl)
  {
  }




void SvNetChannelFile::onFile(SvNetChannel *ch, const QByteArray bl)
  {
  //Получить файл для записи
  SvNetFile svFile( bl );

  //Путь для размещения файла не пустой, записываем
  QFile file( svFile.mFName );
  if( file.open(QIODevice::WriteOnly) ) {
    file.write( svFile.mContens );

    //Успешное сохранение файла, выслать подтверждение
    svFile.mContens.clear();
    }
  else {
    svFile.mContens.clear();
    svFile.mErrors = 1;
    }
  emit
  }





void SvNetChannelFile::onFileRequest(SvNetChannel *ch, const QByteArray bl)
  {
  answerWithError( CSC_FILE, CSE_FILE_FAIL, QString("Can't write file '%1'").arg(csFile.mFName) );
  }








