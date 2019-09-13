/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Host-система"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Отображение (зеркало) исполняющей системы.

    Содержит копию программы, отражение оперативной памяти, состояние задач (по необходимости)
*/

#ifndef SVMIRROR_H
#define SVMIRROR_H

#include "SvProgramm.h"
#include "SvVMachine/SvVmVpuState.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QElapsedTimer>

//Типы возможных зеркал
#define SMT_UNDEFINED 0 //Зеркало не определено
#define SMT_LOCAL     1 //Исполняющая система на локальной машине внутри данной программы
#define SMT_USB       2 //Исполняющая система в контроллере, подключенном через USB
#define SMT_REMOTE    3 //Исполняющая система доступна через локальную сеть или интернет
#define SMT_STORAGE   4 //Архив с состоянием исполняющей системы




inline QDataStream &operator << ( QDataStream &os, const SvVmVpuState &st ) {
  os << st.mBp << st.mIp << st.mSp << st.mTm << st.mThrow << st.mBaseSp << st.mDebugRun;
  return os;
  }




inline QDataStream &operator >> ( QDataStream &is, SvVmVpuState &st ) {
  is >> st.mBp >> st.mIp >> st.mSp >> st.mTm >> st.mThrow >> st.mBaseSp >> st.mDebugRun;
  return is;
  }





class SvMirror : public QObject
  {
    Q_OBJECT

    QElapsedTimer            mTimer;          //Для отсчета временных интервалов
  protected:
    SvProgrammPtr            mProgramm;       //Скомпилированная программа
    bool                     mLink;           //Состояние связи с реальным исполяемым модулем
    QString                  mLinkStatus;     //Текстовое состояние связи
    bool                     mProcess;        //Состояние активности процесса
    QString                  mProcessStatus;  //Описание текущего этапа
    QString                  mProcessError;   //Ошибка, возникшая в процессе операции
    int                      mRemoteCallId;   //Идентификатор текущей процедуры удаленного вызова
    int                      mRemoteAddr0;    //Адреса, по которым возвращаются параметры
    int                      mRemoteAddr1;
    int                      mRemoteAddr2;
    int                      mRemoteAddr3;

    bool                     mScanTasks;      //Флаг определяет необходимость сканирования задач

  public:
    SvMirror( bool scanTasks );
    virtual ~SvMirror();

    //Тип зеркала
    virtual int           mirrorType() const = 0;

    //Текущая программа
    SvProgrammPtr         getProgramm() { return mProgramm; }

    //Получить состояние подключения
    bool                  getLink() const { return mLink; }
    QString               getLinkStatus() const { return mLinkStatus; }

    //Получить информацию о состоянии процесса
    bool                  getProcess() const { return mProcess; }
    QString               getProcessStatus() const { return mProcessStatus; }
    QString               getProcessError() const { return mProcessError; }


    //Настроить зеркало
    virtual void          settings( const QString ip, int port,
                                    const QString globalName, const QString globalPassw,
                                    int vid, int pid );

    //Выполнить обработку узла
    virtual void          processing( int tickOffset ) = 0;


    //Обработка удаленных вызовов
    void                  proceccingRemoteCall();

    //===========================
    //Раздел списка задач
    virtual int           taskCount() const = 0;
    virtual int           taskMax() const = 0;

    //Получить информацию по задаче
    virtual SvVmVpuState  taskInfo( qint32 taskId ) const = 0;

            //Получить все задачи списком
            QByteArray    taskList();



    //===========================
    //Раздел памяти данных

    //Количество памяти в контроллере
    virtual int           memorySize() const = 0;

    //Получить состояние ячейки памяти
    virtual int           memoryGet( int index ) = 0;

    //Установить состояние ячейки памяти
    virtual void          memorySet( int index, int value ) = 0;

            //Получить всю память
            QByteArray    memory();

            //Получить размер глобальной памяти
            int           memoryGlobalSize() const;

            //Получить только глобальную память
            QByteArray    memoryGlobal();

            //Получить адрес символа
    virtual int           addressOfName( const QString &name ) const;

            int           memoryGetByName( const QString &name ) { return memoryGet( addressOfName(name) ); }

            void          memorySetByName( const QString &name, int value ) { memorySet( addressOfName(name), value ); }




    //===========================
    //Раздел управления отладкой

    //Отладка - пуск
    virtual void          debugRun( int taskId ) = 0;

    //Отладка - пуск всех задач
            void          debugRunAll();

    //Отладка - стоп (пауза)
            void          debugPause( int taskId );

    //Отладка - стоп (пауза) всех задач
            void          debugPauseAll();

    //Отладка - исполнять пока внутри и не изменится bp (шаг)
    virtual void          debugRunStep( int taskId, int start, int stop ) = 0;
            void          debugStep( int taskId );

    //Отладка - исполнять пока снаружи (точка останова)
    virtual void          debugRunUntil( int taskId, int start, int stop ) = 0;
            void          debugBreakPoint( int taskId, const QString fname, int line );

    //Отладка - исполнять пока внутри (трассировка)
    virtual void          debugRunTrace( int taskId, int start, int stop ) = 0;
            void          debugTrace( int taskId );

  signals:
            //Передать блок по каналу связи с системой удаленного управления
            void          sendBlock( const void *chId, int cmd, QByteArray block );

            //Прогресс операции
            void          processChanged( const QString status, bool processStatus, const QString error );

            //Изменилось состояние связи
            void          linkStatusChanged( bool linkStatus, const QString status );

            //При изменении задач
            void          taskChanged();

            //При изменении памяти
            void          memoryChanged();

            //При поступлении loga
            void          log( const QString msg );

            //Требование вызова удаленной процедуры с максимум 4-мя параметрами
            // Идея в следующем: в скрипте есть 9 переменных (идентификатор удаленной процедуры,
            //   4 произвольных параметра и 4 указателя для результата)
            // как только зеркало обнаруживает установленным идентификатор удаленной процедуры,
            // то оно считывает параметры, запоминает указатели для результата и выдает этот сигнал
            // для обработки соответствующими объектами
            // По завершению обработки должен быть вызван слот remoteCallComplete для установления
            // результатов и для завершения удаленного вызова. Завершение удаленного вызова
            // осуществляется сбросом идентификатора удаленной процедуры, сброса которого ожидает скрипт
            void remoteCall( int procId, int p0, int p1, int p2, int p3 );


  public slots:
            //Периодическая функция, обеспечивающая работу зеркала
            void          periodic();

            //При получении блока по каналу связи
            void          onReceivBlock( const void *chId, int cmd, QByteArray block );

    //Завершить вызов удаленной процедуры и вернуть результат
    virtual void          remoteCallComplete( int r0, int r1, int r2, int r3 );

    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void          restart( bool runOrPause ) = 0;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void          setProgrammFlashRun( SvProgrammPtr prog, bool link, bool flash, bool runOrPause ) = 0;


            //Выполнить построение программы перед запуском
            void          compileFlashRun( const QString scriptPath, bool link, bool flash, bool runOrPause );


            //Запустить скрипт на исполнение
            //scriptPath - полный путь к скрипту
            void          startScript( const QString scriptPath );

  protected:
    //Установить новое состояние связи
    void                  setLink( const QString status, bool lnk );

    //Изменить текстовое состояние
    void                  setProcess( const QString status, bool processStatus = true, const QString error = QString() );
  };

using SvMirrorPtr = SvMirror*;

#endif // SVMIRROR_H
