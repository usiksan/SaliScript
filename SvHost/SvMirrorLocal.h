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
    Отображение локальной исполняющей системы. Исполняющая система передается в качестве
    параметра
*/


#ifndef SVMIRRORLOCAL_H
#define SVMIRRORLOCAL_H

#include "SvMirror.h"
#include "SvVMachineLocal.h"

class SvMirrorLocal : public SvMirror
  {
    Q_OBJECT

    SvVMachineLocal   *mController; //! Local controller
    int                mDivider;    //! Divider for change event thinning
  public:
    SvMirrorLocal(SvVMachineLocal *controller, bool scanTasks);
    virtual ~SvMirrorLocal() override;


    //===========================
    //Task list section
    //===========================
    //Task list section
    //!
    //! \brief taskCount Return current active tasks count
    //! \return          Current active tasks count
    //!
    virtual int           taskCount() const override { return mController->taskCount(); }

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

  };

#endif // SVMIRRORLOCAL_H
