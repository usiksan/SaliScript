#include "SvNetChannelFile.h"

#include <QDir>

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
  SvNetDirInfo svDir( bl );

  QDir dir(svDir.mPath);
  svDir.mFileInfoList.clear();
  if( dir.exists() ) {
    //Директорий существует

    //Список файлов каталога
    //data: QString путь к каталогу
    //      QFileInfoList со списком файлов
    //Переписываем файлы из списка QFileInfoList в список CsFileInfoList
    // поскольку первый не сериализуется, а второй сериализуется
    QFileInfoList inList = dir.entryInfoList();
    for( const QFileInfo &inf : inList ) {
      SvNetFileInfo info;
      info.mName = inf.fileName();
      info.mSize = inf.size();
      //TODO добавить флаги
      info.mFlags = 0; //inf.isDir();
      info.mPermissions = static_cast<quint16>( inf.permissions() );
      svDir.mFileInfoList.append( info );
      }
    }
  else svDir.mErrors = SVCE_FC_NO_DIR;

  emit sendBlock( ch, SVC_DIR, svDir.buildBlock() );
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








