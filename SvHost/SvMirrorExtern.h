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
    Базовое зеркало внешних VPU. Содержит отображение памяти и процессов.
*/

#ifndef SVMIRROREXTERN_H
#define SVMIRROREXTERN_H

#include "SvMirror.h"
#include "SvVMachine/SvVmVpuState.h"
#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>


//Задание отладки для задачи
struct SvDebugTask {
    int mCommand; //Команда отладки
    int mParam1;  //Параметр 1 для команды отладки
    int mParam2;  //Параметр 2 для команды отладки

    void set( int cmd, int p1 = 0, int p2 = 0 ) { mCommand = cmd; mParam1 = p1; mParam2 = p2; }
  };

class SvMirrorExtern : public SvMirror
  {
    Q_OBJECT

  protected:
    SvVmVpuState  *mVpuState;    //Зеркало состояния виртуальных машин
    SvDebugTask   *mVpuDebug;    //Команды отладочного управления
    QMutex         mVpuMutex;    //Механизм защиты от сдвоенного доступа к командам управления
    int            mVpuCount;    //Количество работающих VPU
    int           *mMemory;      //Зеркало памяти
    int            mMemorySize;  //Размер памяти данных
    int            mMemoryCount; //Размер памяти глобальных переменных

    //очередь переменных на запись в контроллер
    QList<int>     mWriteQueue;  //Индексы переменных, значения которых нужно записать
    QMap<int, int> mWriteValues; //Перечень записываемых значений индекс(ключ)-значение
    QMutex         mWriteMutex;  //Механизм защиты от сдвоенного доступа

    int mItemCount;
    int mReceivTimeOut;
    bool mFlash;
    bool mReset;
    bool mRun;
    unsigned char mBuf[64];
    char mLog[1024];
    int  mLogLen;
  public:
    SvMirrorExtern( bool scanTasks );
    virtual ~SvMirrorExtern() override;

    //===========================
    //Task list section
    virtual int           taskCount() const override { return mVpuCount; }

    //!
    //! \brief taskInfo      Get task information for task with id equals taskId
    //! \param taskId        Index of task whose information need to get
    //! \param destTaskInfo  Reference to VpuState - task information structure
    //! \return              True if successfull or false in other cases
    //!
    virtual bool          taskInfo( qint32 taskId, SvVmVpuState &destTaskInfo ) const override;



    //===========================
    //Memory section
    //!
    //! \brief memoryGet Get mirror memory cell value
    //! \param index     Cell index, whoes value need get
    //! \return          Cell value
    //!
    virtual int           memoryGet( int index ) override;

    //!
    //! \brief memorySet Set mirror memory cell value
    //! \param index     Cell index, whoes value need set
    //! \param value     Value which set
    //!
    virtual void          memorySet( int index, int value ) override;



    //===========================
    //Debug section
    //!
    //! \brief debug     Execute one debug command
    //! \param taskId    Index of task on which must be executed debug command
    //! \param debugCmd  Debug command code
    //! \param start     Start address needed for debug
    //! \param stop      Stop address needed for debug
    //!
    virtual void          debug( int taskId, int debugCmd, int start, int stop ) override;


  public slots:
    //===========================
    //Control section

    //!
    //! \brief restart    At first - reset, then root virtual machine creation and run from start address
    //! \param runOrPause If true - run from start address, else - paused
    //!
    virtual void          restart( bool runOrPause ) override;

    //!
    //! \brief setProgrammFlashRun Flash programm to controller and run it or paused
    //! \param prog                Programm which flashed to controller
    //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
    //!
    virtual void          setProgrammFlashRun( SvProgrammPtr prog, bool runOrPause ) override;

  protected:
    //!
    //! \brief processing Perform periodic mirror handle
    //! \param tickOffset Time in ms between previous calling this function and this one
    //!
    virtual void          processing( int tickOffset ) override;

    virtual void          send( const unsigned char *buf, int len ) = 0;

            void          onReceived(const unsigned char *buf );

  protected:
            void          parseVersion( const unsigned char *buf );
            void          parseState( const unsigned char *buf );
            void          parseVariables( const unsigned char *buf );
            void          parseTask( const unsigned char *buf );
            void          parseFlash( const unsigned char *buf );

    void queryMemory();
    void queryNextMemory();
    void queryNextTask();
    void queryNextDebug();
    void queryNextFlash();
    void queryEraseFlash();

    void queryState();
    void queryReset();
    void queryFlashRead();
    void queryRestart();
    void queryVersion();

  };

#endif // SVMIRROREXTERN_H
