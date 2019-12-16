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

#include "SvVMachineLocal.h"
//#include "SvMirror.h"

#include <QDebug>

SvVMachineLocal::SvVMachineLocal(int ramSize, int vpuCount)
  {
  mProgramm.reset( new SvProgramm() );
  //Распределить память
  mMemory = new int[ramSize];
  mMemorySize = ramSize;

  mVpuList = new SvVmVpu[vpuCount];
  mVpuMax = vpuCount;
  }




SvVMachineLocal::~SvVMachineLocal()
  {
  if( mMemory ) delete mMemory;
  if( mVpuList ) delete mVpuList;
  }







void SvVMachineLocal::memFail(SvVmVpu *vpu, int index) const
  {
  qDebug() << "SvVMachineLocal memFail at " << index;
  Q_UNUSED(vpu)
  }





void SvVMachineLocal::progFail(SvVmVpu *vpu, int address, int len)
  {
  qDebug() << "SvVMachineLocal progFail at " << address << " with len " << len;
  reset();
  Q_UNUSED(vpu)
  }





//Установить программу для исполнения локальным контроллером
void SvVMachineLocal::setProgrammPtr(SvProgrammPtr prog)
  {
  mProgramm = prog;
  setProgramm( mProgramm->getVpuCode() );
  }




//Получить состояние переменной по ее имени
int SvVMachineLocal::variableGet(const QString name, int index) const
  {
  if( mProgramm ) {
    return memGet( nullptr, mProgramm->getAddr(name) + index );
    }
  return 0;
  }




//Установить новое состояние переменной по ее имени
void SvVMachineLocal::variableSet(int value, const QString name, int index)
  {
  if( mProgramm )
    memSet( nullptr, mProgramm->getAddr(name) + index, value );
  }
