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
#ifndef SVNETSERVICEFILESLAVE_H
#define SVNETSERVICEFILESLAVE_H

#include "SvNetService.h"


#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QDateTime>

#define SVC_FILE_ANSWER             10 //File slave answer
                                       //data: SvNetFileAnswer

#define SVC_FILE_REQUEST            11 //File query
                                       //data: SvNetFile
#define SVC_FILE                    12 //File contents
                                       //data: SvNetFile

#define SVC_FILE_DIR_REQUEST        13 //Query to get file list available in directory
                                       //data: SvNetDirInfo
#define SVC_FILE_DIR                14 //Directory file list
                                       //data: SvNetDirInfo

#define SVC_FILE_MAKE_DIR           15 //Query to create directory
                                       //data: SvNetDirOper
#define SVC_FILE_REMOVE             16 //Query to remove file or directory (reversive)
                                       //data: SvNetDirOper
#define SVC_FILE_COPY               17 //Query to copy file
                                       //data: SvNetDirOper
#define SVC_FILE_MOVE               18 //Query to move file
                                       //data: SvNetDirOper


//========================================================================================
// Error codes
#define SVCE_FILE_OK                 0 //Операция выполнена успешно
#define SVCE_FILE_NO_DIRECTORY       1 //Указанный директорий не найден
#define SVCE_FILE_CANT_READ          2
#define SVCE_FILE_CANT_WRITE         3
#define SVCE_FILE_CANT_CREATE_DIR    4
#define SVCE_FILE_NO_FILE_OR_DIR     5
#define SVCE_FILE_CANT_REMOVE_DIR    6
#define SVCE_FILE_CANT_REMOVE_FILE   7
#define SVCE_FILE_NOT_A_FILE_OR_DIR  8
#define SVCE_FILE_CANT_COPY_FILE     9
#define SVCE_FILE_CANT_MOVE_FILE    10




struct SvNetFileOper {
    QString mSource;
    QString mDest;

    SvNetFileOper( const QString src ) : mSource(src) {}
    SvNetFileOper( const QString src, const QString dst ) : mSource(src), mDest(dst) {}
    SvNetFileOper( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetFileOper object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mSource
         << mDest;
      return ar;
      }

    //Deserialise SvNetFileOper object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mSource
         >> mDest;
      }
  };

//========================================================================================
// Answer from SvNetServiceFileSlave
struct SvNetFileAnswer {
    QString mMsg;
    quint32 mErrors;

    SvNetFileAnswer( const QString msg, quint32 err = 0 ) : mMsg(msg), mErrors(err) {}
    SvNetFileAnswer( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetFileAnswer object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mMsg
         << mErrors;
      return ar;
      }

    //Deserialise SvNetFileAnswer object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mMsg
         >> mErrors;
      }
  };



//========================================================================================
// Директории - возможность удаленного просмотра

#define SV_NET_FF_DIR        0x01
#define SV_NET_FF_SYMLINK    0x02
#define SV_NET_FF_EXECUTABLE 0x04
#define SV_NET_FF_HIDDEN     0x08
#define SV_NET_FF_READABLE   0x10
#define SV_NET_FF_WRITABLE   0x20

//!
//! \brief The SvNetFileInfo struct Информация о файле для передачи по каналу
//!
struct SvNetFileInfo {
    qint64    mSize;
    quint16   mPermissions;
    quint16   mFlags;
    QDateTime mTime;
    QString   mName;
  };


//Serialise SvNetFileInfo object
inline QDataStream& operator << ( QDataStream &os, const SvNetFileInfo &info ) {
  os << info.mSize
     << info.mPermissions
     << info.mFlags
     << info.mTime
     << info.mName;
  return os;
  }

//Deserialise SvNetFileInfo object
inline QDataStream& operator >> ( QDataStream &is, SvNetFileInfo &info ) {
  is >> info.mSize
     >> info.mPermissions
     >> info.mFlags
     >> info.mTime
     >> info.mName;
  return is;
  }


struct SvNetDirInfo {
    QString              mPath;         //! Directory path
    QList<SvNetFileInfo> mFileInfoList; //! Directory file list

    SvNetDirInfo( QString path ) : mPath(path) {}
    SvNetDirInfo( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetDirInfo object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mPath
         << mFileInfoList;
      return ar;
      }

    //Deserialise SvNetDirInfo object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mPath
         >> mFileInfoList;
      }

  };




//========================================================================================
// Структуры для обмена файлами
//!
//! \brief The SvFile struct Is the file for by net transfer
//!
struct SvNetFile {
    QString    mFName;     //! File name
    qint64     mOffset;    //! For file block transfer this is block offset from file begin
    QByteArray mContent;   //! File content
    qint32     mBlockSize; //! For file block transert this is block size

    SvNetFile( const QString name, qint64 off = 0, qint32 size = 0 ) : mFName(name), mOffset(off), mBlockSize(size) {}
    SvNetFile( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvFile object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mFName
         << mOffset
         << mBlockSize
         << mContent;
      return ar;
      }

    //Deserialise SvFile object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mFName
         >> mOffset
         >> mBlockSize
         >> mContent;
      }

  };





class SvNetServiceFileSlave : public SvNetService
  {
    Q_OBJECT


  public:
    SvNetServiceFileSlave();

  public slots:
    virtual void onReceivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block );

    static SvNetDirInfo getDirInfo( const QString path );
  private:
    void sendAnswer(SvNetChannel *ch, quint32 errCode, const QString msg );

    void fileRequest( SvNetChannel *ch, const SvNetFile &src );

    void file( SvNetChannel *ch, const SvNetFile &src );

    void dirRequest( SvNetChannel *ch, const SvNetDirInfo &src );

    void makeDir( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileRemove( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileCopy( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileMove( SvNetChannel *ch, const SvNetFileOper &oper );
  };

#endif // SVNETSERVICEFILESLAVE_H
