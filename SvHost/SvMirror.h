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
#include <QPointer>


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


using SvVpuVector = QVector<SvVmVpuState>;




class SvMirror : public QObject
  {
    Q_OBJECT

  protected:
    bool mScanTasks;      //! True if needed to scan tasks. Without this debug is impossible. [Флаг определяет необходимость сканирования задач]

  public:
    SvMirror();
    virtual ~SvMirror();

    //!
    //! \brief controllerType Return current linked controller type name string
    //! \return Current linked controller type name string. If no controller linked
    //!         then empty string returned
    //!
    virtual QString     controllerType() const = 0;

    //!
    //! \brief programmName Return programm name loaded into controller
    //! \return Programm name loaded into controller
    //!
    virtual QString     programmName() const = 0;

    //!
    //! \brief vpuVector Return all vpu current status
    //! \return All vpu current status
    //!
    virtual SvVpuVector vpuVector() const = 0;

    //!
    //! \brief addressOfName Return address of symbol or zero if name not defined
    //! \param name          Name which address need to find
    //! \return              Address of symbol name
    //!
    virtual int         addressOfName( const QString name ) const = 0;

    //!
    //! \brief memoryGet Return value of memory cell with index
    //! \param index     Cell index which value will be retrived
    //! \return          Value of cell
    //!
    virtual int         memoryGet( int index ) const = 0;

    //!
    //! \brief memoryGlobalCount Return ram cell memory count for global variables
    //! \return Ram cell memory count for global variables
    //!
    virtual qint32      memoryGlobalCount() const = 0;

    //!
    //! \brief memoryGetByName Return memory cell value by its name
    //! \param name            Name of variable
    //! \return                Value of variable cell
    //!
    int memoryGetByName( const QString name ) const { return memoryGet( addressOfName(name) ); }
  signals:

    void linkChanged( bool linked, const QString controllerType, const QString loadedProgramm );

    //!
    //! \brief transferProcess
    //! \param complete
    //! \param msg
    //!
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
    //! \brief log Signal sended when log emited
    //! \param msg Log message
    //!
    void log( const QString msg );

  public slots:

    //!
    //! \brief linkTo Link to external controller or controller host
    //! \param ipOrUsb IP for local net or bridge. USB file of port (ttyUSB0, com3 and so on)
    //! \param port  Port for local server
    //! \param controllerName Name for remote controller: registered name for bridge or name for USB
    //! \param passw Password for controller iteraction
    //!
    virtual void linkTo( const QString ipOrUsb, int port, const QString controllerName, const QString controllerPassw );

    //!
    //! \brief setProgrammFlashRun Flash programm to controller and run it or paused
    //! \param prog                Programm which flashed to controller
    //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
    //! \param flash               If true then programm flashed into external controller, else - do nothing
    //!
    virtual void setProgrammFlashRun( SvProgrammPtr prog, bool runOrPause, bool flash ) = 0;


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

using SvMirrorPtr = QPointer<SvMirror>;

//Q_DECLARE_METATYPE(SvMirrorPtr)

#endif // SVMIRROR_H
