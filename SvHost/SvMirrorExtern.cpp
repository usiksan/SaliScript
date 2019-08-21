/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorExtern.h"
#include "Vpu/SvVpuUtils.h"

#include <QDebug>

SvMirrorExtern::SvMirrorExtern(bool scanTasks) :
  SvMirror( scanTasks ),
  mVpuState(0),    //Зеркало состояния виртуальных машин
  mVpuDebug(0),
  mVpuMutex(),
  mVpuCount(0),    //Количество VPU
  mVpuMax(0),
  mMemory(0),      //Зеркало памяти
  mMemorySize(0),  //Размер памяти данных
  mMemoryCount(0),
  mWriteMutex()
  {
  }

SvMirrorExtern::~SvMirrorExtern()
  {
  if( mVpuState ) delete mVpuState;
  if( mMemory ) delete mMemory;
  }



//Получить информацию по задаче
bool SvMirrorExtern::taskInfo(int taskId, int *runOrPause, int *ip, int *sp, int *tm, int *bp)
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    if( runOrPause ) *runOrPause = mVpuState[taskId].mIsRun;
    if( ip ) *ip = mVpuState[taskId].mIp;
    if( sp ) *sp = mVpuState[taskId].mSp;
    if( tm ) *tm = mVpuState[taskId].mTm;
    if( bp ) *bp = mVpuState[taskId].mBp;
    return true;
    }
  if( runOrPause ) *runOrPause = 0;
  if( ip ) *ip = 0;
  if( sp ) *sp = 0;
  if( tm ) *tm = 0;
  if( bp ) *bp = 0;
  return false;
  }



//Раздел памяти данных
int SvMirrorExtern::memoryGet(int index)
  {
  if( index > 0 && index < mMemorySize )
    return mMemory[index];
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






//Отладка - пуск
void SvMirrorExtern::debugRun(int taskId)
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( SDC_RUN );
    }
  }



//Отладка - исполнять пока внутри и не изменится bp (шаг)
void SvMirrorExtern::debugRunStep(int taskId, int start, int stop)
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( SDC_RUN_STEP, start, stop );
    }
  }




//Отладка - исполнять пока снаружи (точка останова)
void SvMirrorExtern::debugRunUntil(int taskId, int start, int stop)
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( SDC_RUN_UNTIL, start, stop );
    }
  }




//Отладка - исполнять пока внутри (трассировка)
void SvMirrorExtern::debugRunTrace(int taskId, int start, int stop)
  {
  if( taskId >= 0 && taskId < mVpuCount ) {
    QMutexLocker locker( &mVpuMutex );

    mVpuDebug[taskId].set( SDC_RUN_TRACE, start, stop );
    }
  }






//Построить зеркало в соответствии с удаленным контроллером
void SvMirrorExtern::setupMirror(int vpuMax, int vpuCount, int memoryMax, int memoryCount)
  {
  QMutexLocker locker( &mVpuMutex );
  mVpuMax = svILimit( vpuMax, 1, 256 );
  mVpuCount = svILimit( vpuCount, 0, vpuMax );
  mMemorySize = svILimit( memoryMax, 256, 0x100000 ); //Количество переменных максимальное
  mMemoryCount = svILimit( memoryCount, 0, memoryMax ); //Количество переменных глобальных
  qDebug() << "vpuMax" << mVpuMax << "vpuCount" << mVpuCount << "mMemorySize" << mMemorySize << "mMemoryCount" << mMemoryCount;
  if( mVpuState )
    delete mVpuState;
  mVpuState = new SvVpuState[mVpuMax];
  memset( mVpuState, 0, sizeof(SvVpuState) * mVpuMax );

  if( mVpuDebug )
    delete mVpuDebug;
  mVpuDebug = new SvDebugTask[mVpuMax];
  memset( mVpuDebug, 0, sizeof(SvDebugTask) * mVpuMax );

  if( mMemory )
    delete mMemory;
  mMemory = new int[mMemorySize];
  memset( mMemory, 0, sizeof(int) * mMemorySize );
  }




