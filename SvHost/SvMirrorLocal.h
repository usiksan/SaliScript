/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
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

    SvVMachineLocal   *mController;
    int                mDivider;
  public:
    SvMirrorLocal(SvVMachineLocal *controller, bool scanTasks);
    virtual ~SvMirrorLocal() override;

    //Тип зеркала
    virtual int           mirrorType() const override { return SMT_LOCAL; }

    //Выполнить обработку узла
    virtual void          processing( int tickOffset ) override;


    //===========================
    //Раздел списка задач
    virtual int           taskCount() const override;

    virtual int           taskMax() const override;

    //Получить информацию по задаче
    virtual SvVmVpuState  taskInfo( qint32 taskId ) const override;



    //===========================
    //Раздел памяти данных
    virtual int           memorySize() const override;

    virtual int           memoryGet( int index ) override;

    virtual void          memorySet( int index, int value ) override;



    //===========================
    //Раздел управления отладкой

    //Отладка - пуск
    virtual void          debugRun( int taskId ) override;

    //Отладка - исполнять пока внутри и не изменится bp (шаг)
    virtual void          debugRunStep( int taskId, int start, int stop ) override;

    //Отладка - исполнять пока снаружи (точка останова)
    virtual void          debugRunUntil( int taskId, int start, int stop ) override;

    //Отладка - исполнять пока внутри (трассировка)
    virtual void          debugRunTrace( int taskId, int start, int stop ) override;

  public slots:
    //Завершить вызов удаленной процедуры и вернуть результат
    virtual void          remoteCallComplete( int result ) override;

    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void          restart( bool runOrPause ) override;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void          setProgrammFlashRun( SvProgrammPtr prog, bool link, bool flash, bool runOrPause ) override;
  };

#endif // SVMIRRORLOCAL_H
