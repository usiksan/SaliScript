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

    SvVMachineLocal   *mController;
    int                mDivider;
  public:
    SvMirrorLocal(SvVMachineLocal *controller, bool scanTasks);
    virtual ~SvMirrorLocal() override;

    //Выполнить обработку узла
    virtual void          processing( int tickOffset ) override;


    //===========================
    //Раздел списка задач
    //Получить информацию по задаче
    virtual bool          taskInfo( qint32 taskId, SvVmVpuState &destTaskInfo ) const override;



    //===========================
    //Раздел памяти данных
    virtual int           memoryGet( int index ) override;

    virtual void          memorySet( int index, int value ) override;



    //===========================
    //Раздел управления отладкой

    //Отладка
    virtual void          debug( int taskId, int debugCmd, int start, int stop ) override;


  public slots:
    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void          restart( bool runOrPause ) override;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void          setProgrammFlashRun( SvProgrammPtr prog, bool link, bool flash, bool runOrPause ) override;
  };

#endif // SVMIRRORLOCAL_H
