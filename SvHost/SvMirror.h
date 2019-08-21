/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Отображение (зеркало) исполняющей системы.

    Содержит копию программы, отражение оперативной памяти, состояние задач (по необходимости)
*/

#ifndef SVMIRROR_H
#define SVMIRROR_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include "SvProgramm.h"

//Типы возможных зеркал
#define SMT_UNDEFINED 0 //Зеркало не определено
#define SMT_LOCAL     1 //Исполняющая система на локальной машине внутри данной программы
#define SMT_USB       2 //Исполняющая система в контроллере, подключенном через USB
#define SMT_REMOTE    3 //Исполняющая система доступна через локальную сеть или интернет
#define SMT_STORAGE   4 //Архив с состоянием исполняющей системы

class SvMirror : public QObject
  {
    Q_OBJECT

  protected:
    QString                  mPrjPath;        //Путь к каталогу с исходниками на машине зеркала
    QString                  mMainScript;     //Имя главного скрипта
    SvProgrammPtr            mProgramm;       //Скомпилированная программа
    bool                     mLink;           //Состояние связи с реальным исполяемым модулем
    QString                  mLinkStatus;     //Текстовое состояние связи
    bool                     mProcess;        //Состояние активности процесса
    QString                  mProcessStatus;  //Описание текущего этапа
    QString                  mProcessError;   //Ошибка, возникшая в процессе операции
    int                      mRemoteCallId;   //Идентификатор текущей процедуры удаленного вызова

    bool                     mScanTasks;      //Флаг определяет необходимость сканирования задач

  public:
    SvMirror( bool scanTasks );
    virtual ~SvMirror();

    //Тип зеркала
    virtual int         mirrorType() const = 0;

    //Список исходных файлов
    QStringList         getSourceFileList() const { return mProgramm->mFileList; }

    //Каталог проекта на машине зеркала
    QString             getPrjPath() const { return mPrjPath; }

    //Текущая программа
    SvProgrammPtr       getProgramm() { return mProgramm; }

    //Получить состояние подключения
    bool                getLink() const { return mLink; }
    QString             getLinkStatus() const { return mLinkStatus; }

    //Получить информацию о состоянии процесса
    bool                getProcess() const { return mProcess; }
    QString             getProcessStatus() const { return mProcessStatus; }
    QString             getProcessError() const { return mProcessError; }


    //Настроить зеркало
    virtual void        settings( const QString prjPath, const QString mainScript,
                                  const QString ip, int port,
                                  const QString globalName, const QString globalPassw,
                                  int vid, int pid );

    //Выполнить обработку узла
    virtual void        processing( int tickOffset ) = 0;


    //===========================
    //Раздел списка задач
    virtual int         taskCount() const = 0;
    virtual int         taskMax() const = 0;

    //Получить информацию по задаче
    virtual bool        taskInfo( qint32 taskId, qint32 *runOrPause, qint32 *ip, qint32 *sp, qint32 *tm, qint32 *bp ) = 0;

            //Получить все задачи списком
            QByteArray  taskList();



    //===========================
    //Раздел памяти данных

    //Количество памяти в контроллере
    virtual int         memorySize() const = 0;

    //Получить состояние ячейки памяти
    virtual int         memoryGet( int index ) = 0;

    //Установить состояние ячейки памяти
    virtual void        memorySet( int index, int value ) = 0;

            //Получить всю память
            QByteArray  memory();

            //Получить размер глобальной памяти
            int         memoryGlobalSize() const;

            //Получить только глобальную память
            QByteArray  memoryGlobal();

            //Получить адрес символа
    virtual int         addressOfName( const QString &name ) const;

            int         memoryGetByName( const QString &name ) { return memoryGet( addressOfName(name) ); }

            void        memorySetByName( const QString &name, int value ) { memorySet( addressOfName(name), value ); }




    //===========================
    //Раздел управления отладкой

    //Отладка - пуск
    virtual void        debugRun( int taskId ) = 0;

    //Отладка - пуск всех задач
            void        debugRunAll();

    //Отладка - стоп (пауза)
            void        debugPause( int taskId );

    //Отладка - стоп (пауза) всех задач
            void        debugPauseAll();

    //Отладка - исполнять пока внутри и не изменится bp (шаг)
    virtual void        debugRunStep( int taskId, int start, int stop ) = 0;
            void        debugStep( int taskId );

    //Отладка - исполнять пока снаружи (точка останова)
    virtual void        debugRunUntil( int taskId, int start, int stop ) = 0;
            void        debugBreakPoint( int taskId, const QString fname, int line );

    //Отладка - исполнять пока внутри (трассировка)
    virtual void        debugRunTrace( int taskId, int start, int stop ) = 0;
            void        debugTrace( int taskId );

  signals:
            //Прогресс операции
            void processChanged( const QString status, bool processStatus, const QString error );

            //Изменилось состояние связи
            void linkStatusChanged( bool linkStatus, const QString status );

            //При изменении задач
            void taskChanged();

            //При изменении памяти
            void memoryChanged();

            //При поступлении loga
            void log( const QString msg );

            //Требование вызова удаленной процедуры с максимум 8-ю параметрами
            void remoteCall( int procId, int p0, int p1, int p2, int p3, int p4, int p5 );


  public slots:
    //Завершить вызов удаленной процедуры и вернуть результат
    virtual void        remoteCallComplete( int result );

    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void        restart( bool runOrPause ) = 0;

    //Отправить содержимое проекта
    virtual void        sendProject() = 0;

    //Получить содержимое проекта
    virtual void        receivProject() = 0;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void        compileFlashRun( bool link, bool flash, bool runOrPause ) = 0;



            //Запустить скрипт на исполнение
            //scriptPath - полный путь к скрипту
            void        startScript( const QString scriptPath );

  protected:
    //Установить новое состояние связи
    void                setLink( const QString status, bool lnk );

    //Изменить текстовое состояние
    void                setProcess( const QString status, bool processStatus = true, const QString error = QString() );

    //Построить программу
    void                make();

  };

#endif // SVMIRROR_H
