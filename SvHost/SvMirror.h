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
#include "SvNet/SvNetService.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QElapsedTimer>


#define SDC_NONE            0 //Нет команд управления для данного VPU
#define SDC_RUN             1 //Команда пуск (отключение отладки)
#define SDC_RUN_STEP        2 //Выполнить шаг
#define SDC_RUN_UNTIL       3 //Выполнять, пока IP вне диапазона адресов
#define SDC_RUN_TRACE       4 //Выполнить шаг со входом в подпрограмму


inline QDataStream &operator << ( QDataStream &os, const SvVmVpuState &st ) {
  os << st.mBp << st.mIp << st.mSp << st.mTm << st.mThrow << st.mBaseSp << st.mDebugRun;
  return os;
  }




inline QDataStream &operator >> ( QDataStream &is, SvVmVpuState &st ) {
  is >> st.mBp >> st.mIp >> st.mSp >> st.mTm >> st.mThrow >> st.mBaseSp >> st.mDebugRun;
  return is;
  }


struct SvControllerInfo {
    bool    mLink = false;
    QString mLinkStatus;
    QString mType;
    qint32  mVpuMax = 0;
    qint32  mProgrammMaxSize = 0;
    qint32  mVariableMaxCount = 0;
    qint32  mVersion = 0;
    QString mSignature;
  };




class SvMirror : public QObject
  {
    Q_OBJECT

  protected:
    //Копия скомпилированной программы
    SvProgrammPtr            mProgramm;       //Скомпилированная программа

    //Удаленный вызов процедур
    int                      mRemoteCallId;   //Идентификатор текущей процедуры удаленного вызова
    int                      mRemoteAddr0;    //Адреса, по которым возвращаются параметры
    int                      mRemoteAddr1;
    int                      mRemoteAddr2;
    int                      mRemoteAddr3;

    //Флаг необходимости сканирования задач
    bool                     mScanTasks;      //Флаг определяет необходимость сканирования задач

    SvControllerInfo         mControllerInfo; //
  public:
    SvMirror( bool scanTasks );
    virtual ~SvMirror();

    //Текущая программа
    SvProgrammPtr           getProgramm() { return mProgramm; }

    //Получить информацию о контроллере
    const SvControllerInfo &controllerInfo() const { return mControllerInfo; }

    //Настроить зеркало
    virtual void            settings( const QString ip, int port,
                                      const QString globalName, const QString globalPassw,
                                      int vid, int pid );


    //Обработка удаленных вызовов
    void                    proceccingRemoteCall();

    //===========================
    //Раздел списка задач
    virtual int             taskCount() const = 0;

    //Получить информацию по задаче
    virtual bool            taskInfo( qint32 taskId, SvVmVpuState &destTaskInfo ) const = 0;

            //Получить все задачи списком
            QByteArray      taskList();



    //===========================
    //Раздел памяти данных

    //Получить состояние ячейки памяти
    virtual int             memoryGet( int index ) = 0;

    //Установить состояние ячейки памяти
    virtual void            memorySet( int index, int value ) = 0;

            //Получить размер глобальной памяти
            int             memoryGlobalSize() const;

            //Получить только глобальную память
            QByteArray      memoryGlobal();

            //Получить адрес символа
            int             addressOfName( const QString &name ) const;

            int             memoryGetByName( const QString &name ) { return memoryGet( addressOfName(name) ); }

            void            memorySetByName( const QString &name, int value ) { memorySet( addressOfName(name), value ); }




    //===========================
    //Раздел управления отладкой

    //Отладочная команда
    virtual void            debug( int taskId, int debugCmd, int start, int stop ) = 0;

            void            debugRun( int taskId );

            //Отладка - пуск всех задач
            void            debugRunAll();

            //Отладка - стоп (пауза)
            void            debugPause( int taskId );

            //Отладка - стоп (пауза) всех задач
            void            debugPauseAll();

            //Отладка - исполнять пока внутри и не изменится bp (шаг)
            void            debugStep( int taskId );

            //Отладка - исполнять пока внутри (трассировка)
            void            debugTrace( int taskId );

  signals:
            //Процесс передачи
            // Пока complete - ложь, msg - отражает происходящий процесс передачи
            // Когда complete - истина, то если msg не пустой, то он показывает ошибку, иначе все ок
            void            transferProcess( bool complete, const QString msg );

            //Изменилось состояние связи
            void            controllerInfoChanged( SvMirror *mirrorPtr );

            //При изменении задач
            void            taskChanged();

            //При изменении памяти
            void            memoryChanged();

            //При поступлении loga
            void            log( const QString msg );

            //Требование вызова удаленной процедуры с максимум 4-мя параметрами
            // Идея в следующем: в скрипте есть 9 переменных (идентификатор удаленной процедуры,
            //   4 произвольных параметра и 4 указателя для результата)
            // как только зеркало обнаруживает установленным идентификатор удаленной процедуры,
            // то оно считывает параметры, запоминает указатели для результата и выдает этот сигнал
            // для обработки соответствующими объектами
            // По завершению обработки должен быть вызван слот remoteCallComplete для установления
            // результатов и для завершения удаленного вызова. Завершение удаленного вызова
            // осуществляется сбросом идентификатора удаленной процедуры, сброса которого ожидает скрипт
            void            remoteCall( int procId, int p0, int p1, int p2, int p3 );


  public slots:
            //Завершить вызов удаленной процедуры и вернуть результат
            void            remoteCallComplete( int r0, int r1, int r2, int r3 );

    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void            restart( bool runOrPause ) = 0;

            //!
            //! \brief setProgrammFlashRun Flash programm to controller and run it or paused
            //! \param prog                Programm which flashed to controller
            //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
            //!
    virtual void            setProgrammFlashRun( SvProgrammPtr prog, bool runOrPause ) = 0;


            //Выполнить построение программы перед запуском
            //!
            //! \brief compileFlashRun Perform script compilation and some other tasks defined by params
            //! \param scriptPath      Full script path. We start compilation of this main script
            //! \param link            If flag is set, then mirror linked to controller
            //! \param flash
            //! \param runOrPause
            //!
            void            compileFlashRun( const QString scriptPath, bool link, bool flash, bool runOrPause );


            //!
            //! \brief startScript Perofrm script compilation, load to controller and start script to running
            //! \param scriptPath  Full script path
            //!
            void            startScript( const QString scriptPath );


    //!
    //! \brief processing Perform periodic mirror handle
    //! \param tickOffset Time in ms between previous calling this function and this one
    //!
    virtual void            processing( int tickOffset ) = 0;
  };

using SvMirrorPtr = SvMirror*;

#endif // SVMIRROR_H
