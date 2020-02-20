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
#include "SvNet/SvNetMirror.h"

#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>


class SvMirrorExtern : public SvMirror
  {
    Q_OBJECT

  protected:
    //Копия скомпилированной программы
    SvProgrammPtr         mProgramm;    //! Current programm [Скомпилированная программа]
    SvVpuVector           mVpuState;    //! Mirror of state of virtual machines (tasks) [Зеркало состояния виртуальных машин]
    QVector<SvDebugTask>  mVpuDebug;    //! Debug commands for every virtual machines (tasks) [Команды отладочного управления]
    QMutex                mVpuMutex;    //! Lock mech [Механизм защиты от сдвоенного доступа к командам управления]
    int                   mVpuCount;    //Количество работающих VPU
    QVector<int>          mMemory;      //Зеркало памяти
    int                   mMemoryCount; //Размер памяти глобальных переменных

    //очередь переменных на запись в контроллер
    QList<int>            mWriteQueue;  //Индексы переменных, значения которых нужно записать
    QMap<int, int>        mWriteValues; //Перечень записываемых значений индекс(ключ)-значение
    QMutex                mWriteMutex;  //Механизм защиты от сдвоенного доступа

    QString               mControllerType;
    QString               mProgrammName;
    bool                  mNeedFlash;
    bool                  mRunOrPause;
  public:
    SvMirrorExtern();
    virtual ~SvMirrorExtern() override;

    //!
    //! \brief controllerType Return current linked controller type name string
    //! \return Current linked controller type name string. If no controller linked
    //!         then empty string returned
    //!
    virtual QString     controllerType() const override { return mControllerType; }

    //!
    //! \brief programmName Return programm name loaded into controller
    //! \return Programm name loaded into controller
    //!
    virtual QString     programmName() const override { return mProgrammName; }

    //!
    //! \brief vpuVector Return all vpu current status
    //! \return All vpu current status
    //!
    virtual SvVpuVector vpuVector() const override { return mVpuState; }

    //!
    //! \brief addressOfName Return address of symbol or zero if name not defined
    //! \param name          Name which address need to find
    //! \return              Address of symbol name
    //!
    virtual int         addressOfName( const QString name ) const override;

    //!
    //! \brief memoryGet Return value of memory cell with index
    //! \param index     Cell index which value will be retrived
    //! \return          Value of cell
    //!
    virtual int         memoryGet( int index ) const override;

    //!
    //! \brief memoryGlobalCount Return ram cell memory count for global variables
    //! \return Ram cell memory count for global variables
    //!
    virtual qint32      memoryGlobalCount() const override { return mMemoryCount; }

  public slots:

    //!
    //! \brief memorySet Set memory cell new value
    //! \param index     Memory cell index
    //! \param value     Memory cell value
    //!
    virtual void        memorySet( int index, int value ) override;

    //!
    //! \brief debug     Execute one debug command
    //! \param taskId    Task index for which debug command
    //! \param debugCmd  Debug command code
    //! \param start     Start address (used some debug commands)
    //! \param stop      Stop address (used some debug commands)
    //!
    virtual void        debug( int taskId, int debugCmd, int start, int stop ) override;

  protected:
    void clearDebug();
  };

#endif // SVMIRROREXTERN_H
