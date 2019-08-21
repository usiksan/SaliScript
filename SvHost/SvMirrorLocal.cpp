/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorLocal.h"

//#include <QDebug>

SvMirrorLocal::SvMirrorLocal(SvControllerLocal *controller, bool scanTasks ) :
  SvMirror( scanTasks ),
  mController(controller),
  mDivider(0)
  {
  mLink = true;
  //Заполнить строку статуса
  mLinkStatus = QString("Local controller, ram %1, max vpu %2").arg( mController->getMemorySize() ).arg( mController->getVpuMax() );
  }


SvMirrorLocal::~SvMirrorLocal()
  {
  delete mController;
  //  qDebug() <<"mirror destroyed";
  }




//Выполнить обработку узла
void SvMirrorLocal::processing(int tickOffset)
  {
   // qDebug() <<"mirror processing";
  mController->processing( tickOffset );
  if( mDivider++ >= 10 ) {
    mDivider = 0;
    //qDebug() <<"mirror memoryChanged";
    emit memoryChanged();
    emit taskChanged();
    }
  }




int SvMirrorLocal::taskCount() const
  {
  return mController->taskCount();
  }




//Получить информацию по задаче
bool SvMirrorLocal::taskInfo(int taskId, int *runOrPause, int *ip, int *sp, int *tm, int *bp)
  {
  if( taskId >= 0 && taskId < mController->taskCount() ) {
    const SvVpuCore *vpu = mController->vpu(taskId);
    if( runOrPause ) *runOrPause = vpu->isRun();
    if( ip ) *ip = vpu->mIp;
    if( sp ) *sp = vpu->mSp;
    if( tm ) *tm = vpu->mTm;
    if( bp ) *bp = vpu->mBp;
    return true;
    }
  if( runOrPause ) *runOrPause = 0;
  if( ip ) *ip = 0;
  if( sp ) *sp = 0;
  if( tm ) *tm = 0;
  if( bp ) *bp = 0;
  return false;
  }







//Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
void SvMirrorLocal::restart(bool runOrPause)
  {
  mController->restart( runOrPause );
  }





int SvMirrorLocal::memoryGet(int index)
  {
  return mController->memGet( 0, index );
  }



void SvMirrorLocal::memorySet(int index, int value)
  {
  mController->memSet( 0, index, value );
  }



//Отладка - пуск
void SvMirrorLocal::debugRun(int taskId)
  {
  mController->debugRun( taskId );
  }




//Отладка - исполнять пока внутри и не изменится bp (шаг)
void SvMirrorLocal::debugRunStep(int taskId, int start, int stop)
  {
  mController->debugRunStep( taskId, start, stop );
  }



//Отладка - исполнять пока снаружи (точка останова)
void SvMirrorLocal::debugRunUntil(int taskId, int start, int stop)
  {
  mController->debugRunUntil( taskId, start, stop );
  }



//Отладка - исполнять пока внутри (трассировка)
void SvMirrorLocal::debugRunTrace(int taskId, int start, int stop)
  {
  mController->debugRunTrace( taskId, start, stop );
  }




//Завершить вызов удаленной процедуры и вернуть результат
void SvMirrorLocal::remoteCallComplete(int result)
  {
  }





//Отправить содержимое проекта
void SvMirrorLocal::sendProject()
  {
  //Отправим сигнал о завершении операции, поскольку для локальной копии не актуально
  setProcess( tr("Complete"), false );
  }





//Получить содержимое проекта
void SvMirrorLocal::receivProject()
  {
  //Отправим сигнал о завершении операции, поскольку для локальной копии не актуально
  setProcess( tr("Complete"), false );
  }






//Компиляция, прошивка и запуск
void SvMirrorLocal::compileFlashRun(bool link, bool flash, bool runOrPause)
  {
  //Отправим сигнал о начале операции
  setProcess( tr("Compile %1 in %2").arg(mMainScript).arg(mPrjPath) );

  //Выполнить компиляцию
  make();
  if( mProgramm->mErrors.count() ) {
    //При наличии ошибок прекращаем
    setProcess( tr("Complete"), false, tr("Compile errors") );
    return;
    }

  if( link ) {
    //Проверить равенство программы только если программа не установлена
    if( mController->getProgramm() == nullptr )
      flash = true;
    else
      flash = mProgramm->getCode() != mController->getProgramm()->getCode();
    }
  if( flash ) {
    setProcess( tr("Setting programm") );

    //Установить программу в исполняющую систему
    mController->setProgrammPtr( mProgramm );

    //Запускаем контроллер
    mController->restart( runOrPause );
    }
  setProcess( tr("Complete"), false );
  }






int SvMirrorLocal::taskMax() const
  {
  return mController->getVpuMax();
  }






int SvMirrorLocal::memorySize() const
  {
  return mController->getMemorySize();
  }

