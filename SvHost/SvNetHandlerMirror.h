/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Обработчик обмена с зеркалом. Основное назначение - клиент для удаленного управления зеркалом.
*/
#ifndef SVNETHANDLERMIRROR_H
#define SVNETHANDLERMIRROR_H

#include "SvNet/SvNetChannel.h"
#include "SvMirror.h"

#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QDateTime>

#define SVCM_VM_CONFIG_REQUEST      40 //Запрос конфигурации зеркала

#define SVCM_VM_CONFIG              41 //Конфигурация зеркала
                                       //data: SvNetMirrorVmConfig

#define SVCM_PROGRAMM_RUN           42 //Программа
                                       //data: SvProgramm

#define SVCM_PROGRAMM_PAUSE         43 //Программа
                                       //data: SvProgramm

#define SVCM_MEMORY_VECTOR          44 //Состояние памяти в диапазоне адресов
                                       //data: SvNetMirrorMemoryVector

#define SVCM_MEMORY_MAP             45 //Установить новое состояние памяти
                                       //data: SvNetMirrorListInt Список пар значений адрес:данные

#define SVCM_DEBUG                  46 //Команды отладки, в ответ присылается состояние задач
                                       //data: SvNetMirrorListInt Список четверок значений задача:команда:параметр1:параметр2

#define SVCM_TASK_LIST              47 //Текущее состояние задач
                                       //data: SvNetMirrorTaskList

//Асинхронные сообщения
#define SVCM_LOG                    47 //Log
                                       //data: SvNetMirrorLog



struct SvNetMirrorVmConfig {
    qint32  mVmMax;       //Максимальное количество процессоров
    qint32  mVmCount;     //Текущее количество процессоров
    qint32  mMemoryMax;   //Размер оперативной памяти зеркала
    qint32  mMemoryCount; //Используемуе количество оперативной памяти зеркала
    QString mProgName;    //Имя программы зеркала
    QString mStatus;      //Строка состояния зеркала

    SvNetMirrorVmConfig( );
  };


struct SvNetMirrorMemoryVector {
    qint32        mStartAddr; //Адрес начала вектора памяти
    qint32        mCount;     //Количество ячеек памяти
    QList<qint32> mVector;    //Вектор ячеек памяти
  };




struct SvNetMirrorListInt {
    QList<qint32> mMap; //Список пар значений адрес:данные
  };



struct SvNetMirrorTaskList {
  QList<SvVmVpuState> mList; //Список задач с их состояниями

  };



struct SvNetMirrorLog {
    QString mLog; //Сообщение
  };





class SvNetHandlerMirror : public QObject
  {
    Q_OBJECT

    SvMirror *mMirror; //! Зеркало для наблюдения
  public:
    explicit SvNetHandlerMirror(QObject *parent = nullptr);

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

    //!
    //! \brief addChannel Добавить канал в который отправлять информацию о зеркале
    //! \param ch
    //!
    void addChannel( SvNetChannel *ch );

    //!
    //! \brief setMirror Установить новое зеркало для наблюдения
    //! \param mirror зеркало
    //!
    void setMirror( SvMirror *mirror );

    //!
    //! \brief onLog Получения сообщения от зеркала. Мы его переправляем в канал
    //! \param log Сообщение от зеркала
    //!
    void onLog( const QString log );

  private:
    void onDirListRequest( SvNetChannel *ch, const QByteArray bl );

    void onFile( SvNetChannel *ch, const QByteArray bl );

    void onFileRequest( SvNetChannel *ch, const QByteArray bl );
  };

#endif // SVNETHANDLERMIRROR_H
