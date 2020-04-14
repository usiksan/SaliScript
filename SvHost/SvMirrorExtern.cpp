/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorExtern.h"
#include "SvVMachine/SvVmUtils.h"

#include <QDebug>

SvMirrorExtern::SvMirrorExtern() :
  SvMirror(),
  mVpuMutex(),
  mMemoryCount(0),
  mWriteMutex(),
  mNeedFlash(false),
  mRunOrPause(true)
  {
  }




SvMirrorExtern::~SvMirrorExtern()
  {
  }




SvVpuVector SvMirrorExtern::vpuVector() const
  {
  SvVpuVector state;
  state.resize( mVpuState.count() );
  for( int i = 0; i < mVpuState.count(); i++ )
    state[i] = mVpuState.at(i);

  return state;
  }




int SvMirrorExtern::addressOfName(const QString name) const
  {
  if( mProgramm.isNull() )
    return 0;
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
  if( 0 <= taskId && taskId < mVpuDebug.count() ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( debugCmd, start, stop );
    }
  }




//Clear debug
void SvMirrorExtern::clearDebug()
  {
  //For each task we clear it
  for( SvDebugTask &deb : mVpuDebug )
    deb.clear();
  }





