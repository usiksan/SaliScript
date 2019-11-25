/*
  Project "Net packet interchange over local net and internet bridge server"
  Автор
    Sibilev A.S.

    www.salilab.com
    www.salilab.ru
  Description
    The service interacts with the channel to perform a file operations.
    This service is master service, i.e. it initiates interhange but SvNetServiceFileSlave handle this queries.
*/
#include "SvNetServiceFileSlave.h"

#include <QFile>
#include <QDir>

SvNetServiceFileSlave::SvNetServiceFileSlave()
  {

  }



void SvNetServiceFileSlave::sendAnswer(SvNetChannel *ch, quint32 errCode, const QString msg)
  {
  emit sendBlock( ch, SVC_FILE_ANSWER, SvNetFileAnswer( msg, errCode ).buildBlock() );
  }



void SvNetServiceFileSlave::fileRequest(SvNetChannel *ch, const SvNetFile &src)
  {
  QFile file(src.mFName);
  if( file.open(QIODevice::ReadOnly) ) {
    SvNetFile dst(src);
    if( src.mBlockSize ) {
      //Request file block
      file.seek( src.mOffset );
      dst.mContent = file.read(src.mBlockSize);
      }
    else {
      //Request full file
      dst.mContent = file.readAll();
      }
    emit sendBlock( ch, SVC_FILE, dst.buildBlock() );
    }
  else sendAnswer( ch, SVCE_FILE_CANT_READ, QString("Can't open file %1").arg(src.mFName) );
  }






void SvNetServiceFileSlave::file(SvNetChannel *ch, const SvNetFile &src)
  {
  QFile file( src.mFName );
  if( file.open(QIODevice::WriteOnly) ) {
    file.write( src.mContent );
    file.close();
    sendAnswer( ch, SVCE_FILE_OK, QString("File %1 writed").arg(src.mFName) );
    }
  else
    sendAnswer( ch, SVCE_FILE_CANT_WRITE, QString("Can't write file %1").arg(src.mFName) );
  }




void SvNetServiceFileSlave::dirRequest(SvNetChannel *ch, const SvNetDirInfo &src)
  {
  QDir dir(src.mPath);
  SvNetDirInfo dst(src);
  dst.mFileInfoList.clear();
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
      dst.mFileInfoList.append( info );
      }
    emit sendBlock( ch, SVC_FILE_DIR, dst.buildBlock() );
    }
  else
    sendAnswer( ch, SVCE_FILE_NO_DIRECTORY, QString("Directory %1 does not exist").arg(src.mPath) );
  }




void SvNetServiceFileSlave::makeDir(SvNetChannel *ch, const SvNetFileOper &oper)
  {
  QDir dir;
  if( dir.mkpath( oper.mSource ) )
    sendAnswer( ch, SVCE_FILE_OK, QString("Directory created %1").arg(oper.mSource) );
  else
    sendAnswer( ch,  )
  }




