/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorExtern.h"
#include "SvVMachine/SvVmUtils.h"
#include "SvVMachine/SvVmInterface.h"

#include <QDebug>

SvMirrorExtern::SvMirrorExtern() :
  SvMirror(),
  mVpuMutex(),
  mVpuCount(0),    //Количество VPU
  mMemoryCount(0),
  mWriteMutex()
  {
  }




SvMirrorExtern::~SvMirrorExtern()
  {
  }




int SvMirrorExtern::addressOfName(const QString name) const
  {
  return mProgramm->getAddr(name);
  }




int SvMirrorExtern::memoryGet(int index) const
  {
  if( index > 0 && index < mMemory.size() )
    return mMemory.at(index);
  return 0;
  }




//Установить состояние ячейки памяти
void SvMirrorExtern::memorySet(int index, int value)
  {
  //Блокиратор сдвоенного доступа к очереди
  QMutexLocker locker(&mWriteMutex);

  //Если значение с данным индексом уже содержится в очереди
  if( mWriteValues.contains(index) )
    //то его просто меняем
    mWriteValues[index] = value;
  else {
    //Иначе добавляем значение в очередь
    mWriteValues.insert(index, value);
    mWriteQueue.append(index);
    }
  }






void SvMirrorExtern::debug(int taskId, int debugCmd, int start, int stop)
  {
  if( 0 <= taskId && taskId < mVpuCount ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( debugCmd, start, stop );
    }
  }





