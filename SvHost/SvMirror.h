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
#include <QList>
#include <QMutex>
#include <QMutexLocker>


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
    bool mScanTasks;      //! True if needed to scan tasks. Without this debug is impossible. [Флаг определяет необходимость сканирования задач]

  public:
    SvMirror();
    virtual ~SvMirror();

    //!
    //! \brief addressOfName Return address of symbol or zero if name not defined
    //! \param name          Name which address need to find
    //! \return              Address of symbol name
    //!
    virtual int  addressOfName( const QString name ) const = 0;

    //!
    //! \brief memoryGet Return value of memory cell with index
    //! \param index     Cell index which value will be retrived
    //! \return          Value of cell
    //!
    virtual int  memoryGet( int index ) const = 0;
  signals:

    void linkChanged( bool linked, const QString controllerType, const QString loadedProgramm );

    void transferProcess( bool complete, const QString msg );

    //!
    //! \brief programmChanged Signal sended when controller programm changed
    //! \param programm        New controller programm
    //!
    void programmChanged( SvProgrammPtr programm );

    //!
    //! \brief memoryChanged Signal that values of some cell changed. Handler must own retrive
    //!                      single or more cell values with memoryGet function
    //! \param src           Mirror which values are changed
    //!
    void memoryChanged( SvMirror *src );

    //!
    //! \brief taskChanged Signal sended when task changed
    //! \param taskIndex   Task index
    //! \param ip          instruction pointer [указатель инструкций]
    //! \param sp          stack pointer [указатель стека]
    //! \param bp          function frame pointer [указатель базы в стеке локальных переменных для текущей функции (указывает на фрейм возврата из функции)]
    //! \param tm          exception mask [маска обрабатываемых исключений]
    //! \param baseSp      stack start [Начало стека для данного процессора]
    //! \param mthrow      current exception [Текущее значение исключения]
    //! \param debugRun    if eq 0 then in debug state else - in run state
    //!
    void taskChanged( int taskIndex, int ip, int sp, int bp, int tm, int baseSp, int mthrow, int debugRun );

    //!
    //! \brief log Signal sended when log emited
    //! \param msg Log message
    //!
    void log( const QString msg );

  public slots:


    //!
    //! \brief setProgrammFlashRun Flash programm to controller and run it or paused
    //! \param prog                Programm which flashed to controller
    //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
    //!
    virtual void setProgrammFlashRun( SvProgrammPtr prog, bool runOrPause ) = 0;


    //!
    //! \brief compileFlashRun Perform script compilation and some other tasks defined by params
    //! \param scriptPath      Full script path. We start compilation of this main script
    //! \param runOrPause      If true then programm automaticly started after flash, else - it paused
    //!
    void compileFlashRun(const QString scriptPath, bool runOrPause );


    //!
    //! \brief startScript Perofrm script compilation, load to controller and start script to running
    //! \param scriptPath  Full script path
    //!
    void startScript( const QString scriptPath );

    //!
    //! \brief memorySet Set memory cell new value
    //! \param index     Memory cell index
    //! \param value     Memory cell value
    //!
    virtual void memorySet( int index, int value ) = 0;

    //!
    //! \brief debug     Execute one debug command
    //! \param taskId    Task index for which debug command
    //! \param debugCmd  Debug command code
    //! \param start     Start address (used some debug commands)
    //! \param stop      Stop address (used some debug commands)
    //!
    virtual void debug( int taskId, int debugCmd, int start, int stop ) = 0;

    //!
    //! \brief processing Perform periodic mirror handle
    //! \param tickOffset Time in ms between previous calling this function and this one
    //!
    virtual void processing( int tickOffset ) = 0;

    //!
    //! \brief init Called automaticly from inside execution thread to init mirror after creation
    //!
    virtual void init() {}

  };

using SvMirrorPtr = SvMirror*;

#endif // SVMIRROR_H
