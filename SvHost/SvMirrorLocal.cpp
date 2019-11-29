/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorLocal.h"

//#include <QDebug>

SvMirrorLocal::SvMirrorLocal(SvVMachineLocal *controller, bool scanTasks ) :
  SvMirror( scanTasks ),
  mController(controller),
  mDivider(0)
  {
  //Через секунду после создания выполнить подключение
  }



SvMirrorLocal::~SvMirrorLocal()
  {
  delete mController;
  }







//Получить информацию по задаче
bool SvMirrorLocal::taskInfo(qint32 taskId, SvVmVpuState &destTaskInfo) const
  {
  if( taskId >= 0 && taskId < mController->taskCount() ) {
    destTaskInfo = *(mController->vpu(taskId));
    return true;
    }
  return false;
  }









int SvMirrorLocal::memoryGet(int index)
  {
  return mController->memGet( nullptr, index );
  }






void SvMirrorLocal::memorySet(int index, int value)
  {
  mController->memSet( nullptr, index, value );
  }







void SvMirrorLocal::debug(int taskId, int debugCmd, int start, int stop)
  {
  switch( debugCmd ) {
    case SDC_RUN :
      //Команда пуск (отключение отладки)
      mController->debugRun( taskId );
      break;
    case SDC_RUN_STEP :
      //Выполнить шаг
      //Отладка - исполнять пока внутри и не изменится bp (шаг)
      mController->debugRunStep( taskId, start, stop );
      break;
    case SDC_RUN_UNTIL :
      //Выполнять, пока IP вне диапазона адресов
      //Отладка - исполнять пока снаружи (точка останова)
      mController->debugRunUntil( taskId, start, stop );
      break;
    case SDC_RUN_TRACE :
      //Выполнить шаг со входом в подпрограмму
      //Отладка - исполнять пока внутри (трассировка)
      mController->debugRunTrace( taskId, start, stop );
    }
  }












//Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
void SvMirrorLocal::restart(bool runOrPause)
  {
  mController->restart( runOrPause );
  }






//Установка программы, прошивка и запуск
void SvMirrorLocal::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause)
  {
  //Отправим сигнал о начале операции
  emit transferProcess( false, tr("Set programm") );

  //Установить новую программу
  mProgramm = prog;


  if( link ) {
    //Проверить равенство программы только если программа не установлена
    if( mController->getProgramm() == nullptr )
      flash = true;
    else
      flash = mProgramm->getCode() != mController->getProgramm()->getCode();
    }

  if( flash ) {
    emit transferProcess( false, tr("Setup programm to controller") );

    //Установить программу в исполняющую систему
    mController->setProgrammPtr( mProgramm );

    //Запускаем контроллер
    mController->restart( runOrPause );
    }
  emit transferProcess( true, QString() );
  }






//Mirror processing
void SvMirrorLocal::processing(int tickOffset)
  {
  if( !mControllerInfo.mLink ) {
    //At first step we initiate controller connection
    mControllerInfo.mLink        = true;
    mControllerInfo.mType        = tr("Local controller");
    mControllerInfo.mVpuMax      = mController->getVpuMax();
    mControllerInfo.mVpuCount    = 0;
    mControllerInfo.mMemoryMax   = mController->getMemorySize();
    mControllerInfo.mSignature   = QStringLiteral("--------");
    mControllerInfo.mLinkStatus  = tr("Local controller, ram %1, max vpu %2").arg( mController->getMemorySize() ).arg( mController->getVpuMax() );
    mControllerInfo.mMemoryCount = 0;
    emit controllerInfoChanged( this );
    }

   // qDebug() <<"mirror processing";
  //Controller processing
  mController->processing( tickOffset );
  if( mDivider++ >= 10 ) {
    mDivider = 0;
    //qDebug() <<"mirror memoryChanged";
    emit memoryChanged();
    emit taskChanged();
    }

  if( mController->taskCount() != mControllerInfo.mVpuCount ) {
    mControllerInfo.mVpuCount = mController->taskCount();
    emit controllerInfoChanged( this );
    }
  }















