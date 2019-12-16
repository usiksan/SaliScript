/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Обработчик обмена файлами. Основное назначение - клиент для исполнения удаленных заданий по файловому обмену.
*/
#ifndef SVNETHANDLERFILE_H
#define SVNETHANDLERFILE_H

#include "SvNet/SvNetChannel.h"

#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QDateTime>
#if 0
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



class SvNetHandlerFile : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetHandlerFile(QObject *parent = nullptr);

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

    void setChannel( SvNetChannel *ch );

  private:
    void onDirListRequest( SvNetChannel *ch, const QByteArray bl );

    void onFile( SvNetChannel *ch, const QByteArray bl );

    void onFileRequest( SvNetChannel *ch, const QByteArray bl );
  };

#endif // SVNETHANDLERFILE_H
#endif
