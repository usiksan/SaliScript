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
  if( !mControllerInfo.mLink ) {
    //На начальном шаге инициируем подключение контроллера
    mControllerInfo.mLink = true;
    mControllerInfo.mType = QStringLiteral("Local");
    mControllerInfo.mLinkStatus = QStringLiteral("Connected");
    mControllerInfo.mVpuMax = mController->getVpuMax();
   // qDebug() <<"mirror processing";
  mController->processing( tickOffset );
  if( mDivider++ >= 10 ) {
    mDivider = 0;
    //qDebug() <<"mirror memoryChanged";
    emit memoryChanged();
    emit taskChanged();
    }
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








int SvMirrorLocal::memorySize() const
  {
  return mController->getMemorySize();
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











//Завершить вызов удаленной процедуры и вернуть результат
void SvMirrorLocal::remoteCallComplete(int result)
  {
  Q_UNUSED(result)
  }








//Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
void SvMirrorLocal::restart(bool runOrPause)
  {
  mController->restart( runOrPause );
  }




//Установка программы, прошивка и запуск
void SvMirrorLocal::setProgrammFlashRun(SvProgrammPtr prog, bool link, bool flash, bool runOrPause)
  {
  //Отправим сигнал о начале операции
  setProcess( tr("Compile...") );

  //Установить новую программу
  mProgramm = prog;

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














