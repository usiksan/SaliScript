/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Виртуальная машина"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Виртуальная машина для исполнения байт-кода.
    Виртуальная машина содержит в себе память программ, память данных
    и набор виртуальных процессоров.
    Данная реализация обеспечивает локальную машину.
*/

#ifndef SVVMACHINELOCAL_H
#define SVVMACHINELOCAL_H

#include "SvVMachine/SvVMachine.h"
#include "SvHost/SvProgramm.h"


class SvVMachineLocal : public SvVMachine
  {
  protected:
    SvProgrammPtr mProgramm;
  public:
    SvVMachineLocal( int ramSize = 50000, int vpuCount = 32 );
    ~SvVMachineLocal() override;

    //Нарушение доступа к памяти данных
    virtual void  memFail( SvVmVpu *vpu, int index ) override;

    //Нарушение доступа к памяти программ
    virtual void  progFail( SvVmVpu *vpu, int address, int len ) override;

    //Установить программу для исполнения локальным контроллером
    void          setProgrammPtr( SvProgrammPtr prog );

    SvProgrammPtr getProgramm() const { return mProgramm; }
  };

#endif // SVVMACHINELOCAL_H
