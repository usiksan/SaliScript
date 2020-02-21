#include "SvNetHandlerFile.h"

#include <QFile>
#include <QDir>

SvNetHandlerFile::SvNetHandlerFile()
  {

  }

void SvNetHandlerFile::receivedBlock(SvNetChannel *ch, qint8 cmd, QByteArray block)
  {
  switch( cmd ) {
    case SVC_FILE_REQUEST :
      //File query
      fileRequest( ch, SvNetFile(block) );
      break;

    case SVC_FILE  :
      //File contents
      file( ch, SvNetFile(block) );
      break;

    case SVC_FILE_DIR_REQUEST :
      //Query to get file list available in directory
      dirRequest( ch, SvNetDirInfo(block) );
      break;

    case SVC_FILE_MAKE_DIR :
      //Query to create directory
      makeDir( ch, SvNetFileOper(block) );
      break;

    case SVC_FILE_REMOVE :
      //Query to remove file or directory (reversive)
      fileRemove( ch, SvNetFileOper(block) );
      break;

    case SVC_FILE_COPY :
      //Query to copy file
      fileCopy( ch, SvNetFileOper(block) );
      break;

    case SVC_FILE_MOVE :
      //Query to move file
      fileMove( ch, SvNetFileOper(block) );
      break;
    }
  }






SvNetDirInfo SvNetHandlerFile::getDirInfo(const QString path)
  {
  QDir dir(path);
  SvNetDirInfo dst(path);
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
      info.mFlags = (inf.isDir()        ? SV_NET_FF_DIR : 0) |
                    (inf.isSymLink()    ? SV_NET_FF_SYMLINK : 0) |
                    (inf.isExecutable() ? SV_NET_FF_EXECUTABLE : 0) |
                    (inf.isHidden()     ? SV_NET_FF_HIDDEN : 0) |
                    (inf.isReadable()   ? SV_NET_FF_READABLE : 0) |
                    (inf.isWritable()   ? SV_NET_FF_WRITABLE : 0);
      info.mPermissions = static_cast<quint16>( inf.permissions() );
      info.mTime = inf.fileTime(QFileDevice::FileModificationTime);
      dst.mFileInfoList.append( info );
      }
    }
  return dst;
  }




void SvNetHandlerFile::sendAnswer(SvNetChannel *ch, qint8 srcCmd, quint32 errCode, const QString msg)
  {
  emit sendBlock( ch, SVC_FILE_ANSWER, SvNetAnswer( srcCmd, errCode, msg ).buildBlock() );
  }





void SvNetHandlerFile::fileRequest(SvNetChannel *ch, const SvNetFile &src)
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
  else sendAnswer( ch, SVC_FILE_REQUEST, SVCE_FILE_CANT_READ, QString("Can't open file \"%1\"").arg(src.mFName) );
  }





void SvNetHandlerFile::file(SvNetChannel *ch, const SvNetFile &src)
  {
  QFile file( src.mFName );
  if( file.open(QIODevice::WriteOnly) ) {
    file.write( src.mContent );
    file.close();
    sendAnswer( ch, SVC_FILE, SVCE_FILE_OK, QString("File \"%1\" writed").arg(src.mFName) );
    }
  else
    sendAnswer( ch, SVC_FILE, SVCE_FILE_CANT_WRITE, QString("Can't write file \"%1\"").arg(src.mFName) );
  }





void SvNetHandlerFile::dirRequest(SvNetChannel *ch, const SvNetDirInfo &src)
  {
  QDir dir(src.mPath);
  SvNetDirInfo dst = getDirInfo( src.mPath );
  if( dir.exists() )
    emit sendBlock( ch, SVC_FILE_DIR, dst.buildBlock() );
  else
    sendAnswer( ch, SVC_FILE_DIR_REQUEST, SVCE_FILE_NO_DIRECTORY, QString("Directory \"%1\" does not exist").arg(src.mPath) );
  }




void SvNetHandlerFile::makeDir(SvNetChannel *ch, const SvNetFileOper &oper)
  {
  QDir dir;
  if( dir.mkpath( oper.mSource ) )
    sendAnswer( ch, SVC_FILE_MAKE_DIR, SVCE_FILE_OK, QString("Directory created \"%1\"").arg(oper.mSource) );
  else
    sendAnswer( ch, SVC_FILE_MAKE_DIR, SVCE_FILE_CANT_CREATE_DIR, QString("Can't create dir \"%1\"").arg(oper.mSource) );
  }





void SvNetHandlerFile::fileRemove(SvNetChannel *ch, const SvNetFileOper &oper)
  {
  QFileInfo info(oper.mSource);
  if( info.exists() ) {
    if( info.isDir() ) {
      if( QDir().rmdir( oper.mSource ) )
        sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_OK, QString("Directory \"%1\" removed successfully").arg(oper.mSource) );
      else
        sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_CANT_REMOVE_DIR, QString("Can't remove directory \"%1\"").arg(oper.mSource) );
      }
    else if( info.isFile() ) {
      if( QFile::remove( oper.mSource ) )
        sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_OK, QString("File \"%1\" removed successfully").arg(oper.mSource) );
      else
        sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_CANT_REMOVE_FILE, QString("Can't remove file \"%1\"").arg(oper.mSource) );
      }
    else
      sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_NOT_A_FILE_OR_DIR, QString("Not a file or dir \"%1\"").arg(oper.mSource) );
    }
  else
    sendAnswer( ch, SVC_FILE_REMOVE, SVCE_FILE_NO_FILE_OR_DIR, QString("File \"%1\" not exist").arg(oper.mSource) );
  }




void SvNetHandlerFile::fileCopy(SvNetChannel *ch, const SvNetFileOper &oper)
  {
  if( QFile::copy( oper.mSource, oper.mDest ) )
    sendAnswer( ch, SVC_FILE_COPY, SVCE_FILE_OK, QString("Copy \"%1\" to \"%2\" successfully").arg(oper.mSource).arg(oper.mDest) );
  else
    sendAnswer( ch, SVC_FILE_COPY, SVCE_FILE_CANT_COPY_FILE, QString("Can't copy \"%1\" to \"%2\"").arg(oper.mSource).arg(oper.mDest) );
  }




void SvNetHandlerFile::fileMove(SvNetChannel *ch, const SvNetFileOper &oper)
  {
  if( QFile::rename( oper.mSource, oper.mDest ) )
    sendAnswer( ch, SVC_FILE_MOVE, SVCE_FILE_OK, QString("Move \"%1\" to \"%2\" successfully").arg(oper.mSource).arg(oper.mDest) );
  else
    sendAnswer( ch, SVC_FILE_MOVE, SVCE_FILE_CANT_COPY_FILE, QString("Can't move \"%1\" to \"%2\"").arg(oper.mSource).arg(oper.mDest) );
  }


