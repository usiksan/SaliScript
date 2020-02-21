#ifndef SVNETHANDLERFILE_H
#define SVNETHANDLERFILE_H

#include "SvNetHandler.h"

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







#define SVC_FILE_REQUEST            20 //Запрос на получение файла
                                       //data: CsFile
#define CSC_FILE                     4 //Файл
                                       //data: CsFile

#define SVC_DIR_REQUEST              5 //Запрос на получение листинга доступных файлов в каталоге
                                       //data: CsDir
#define SVC_DIR                      6 //Список файлов каталога
                                       //data: CsDir



//========================================================================================
// Коды ошибок
#define SVCE_FC_OK                  0 //Операция выполнена успешно
#define SVCE_FC_NO_DIR              1 //Указанный директорий не найден



//========================================================================================
// Состояние файловой операции
struct SvNetFileStatus {
    QString mMsg;
    quint32 mErrors;

    SvNetFileStatus( const QString msg, quint32 err = 0 ) : mMsg(msg), mErrors(err) {}
    SvNetFileStatus( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetFileStatus object
    QByteArray buildBlock() {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mMsg
         << mErrors;
      return ar;
      }

    //Deserialise SvNetFileStatus object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mMsg
         >> mErrors;
      }
  };



//========================================================================================
// Директории - возможность удаленного просмотра

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
    QString              mPath;         //Путь к директорию
    QList<SvNetFileInfo> mFileInfoList; //Список файлов из директория
    quint32              mErrors;       //! Errors

    SvNetDirInfo( QString path ) : mPath(path), mErrors(0) {}
    SvNetDirInfo( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetDirInfo object
    QByteArray buildBlock() {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mPath
         << mErrors
         << mFileInfoList;
      return ar;
      }

    //Deserialise SvNetDirInfo object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mPath
         >> mErrors
         >> mFileInfoList;
      }

  };




//========================================================================================
// Структуры для обмена файлами
//!
//! \brief The SvFile struct Представляет собой файл для передачи по сети
//!
struct SvNetFile {
    QString    mFName;   //Имя файла
    QByteArray mContens; //Содержимое файла
    quint32    mErrors;  //! Errors

    SvNetFile( const QString name, const QByteArray contens ) : mFName(name), mContens(contens), mErrors(0) {}
    SvNetFile( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvFile object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mFName
         << mErrors
         << mContens;
      return ar;
      }

    //Deserialise SvFile object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mFName
         >> mErrors
         >> mContens;
      }

  };




class SvNetHandlerFile : public SvNetHandler
  {
    Q_OBJECT

  public:
    SvNetHandlerFile();

  signals:
    /*!
       \brief sendBlock Send block with appropriate command
       \param ch        Channel which must send block or nullptr for all connected channels
       \param cmd       Command for block
       \param ar        Data block

       Block is completed part of interchange data. Each block follow with
       block command which descripting block data. Before block transfer it
       prepare with CsPacketInfo struct which contains block command and
       block size follows block data.
     */
    void sendBlock( SvNetChannel *ch, qint8 cmd, QByteArray block );


  public slots:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    void onReceivedBlock( SvNetChannel *ch, int cmd, QByteArray block );

  private:
    void onDirListRequest( SvNetChannel *ch, const QByteArray bl );

    void onFile( SvNetChannel *ch, const QByteArray bl );

    void onFileRequest( SvNetChannel *ch, const QByteArray bl );

  };

#endif // SVNETHANDLERFILE_H
