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

#include "SvVMachine/Sv7Sys.h"
#include "SvVMachineLocal.h"

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


bool SvVMachineLocal::executeMethod(SvVmVpu *vpu, int methodId)
  {
  if( methodId == VPS_LOG ) {
    //                3           2       1       0
    //void svLog( cstring title, int p1, int p2, int p3 );
    //Получить строку
    QString pat = mProgramm->mStrings.at( VPU_GET_FUN_PARAM(3) );
    //Теперь добавляем параметры, если есть
    if( pat.contains("%3") )
      pat = pat.arg(VPU_GET_FUN_PARAM(2)).arg(VPU_GET_FUN_PARAM(1)).arg(VPU_GET_FUN_PARAM(0));
    if( pat.contains("%2") )
      pat = pat.arg(VPU_GET_FUN_PARAM(2)).arg(VPU_GET_FUN_PARAM(1));
    if( pat.contains("%1") )
      pat = pat.arg(VPU_GET_FUN_PARAM(2));
    mLog.append( pat );
    return true;
    }
  return SvVMachine::executeMethod( vpu, methodId );
  }
