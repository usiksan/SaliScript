/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorLocal.h"

#include <QDebug>


SvMirrorLocal::SvMirrorLocal(SvVMachineLocal *controller , const QString &ctrType) :
  SvMirror(),
  mController(controller),
  mDivider(0),
  mVpuDebugRun( mController->getVpuMax(), 0 ),
  mControllerType()
  {
  if( ctrType.isEmpty() )
    mControllerType = tr("Local controller");
  else
    mControllerType = ctrType;
  }



SvMirrorLocal::~SvMirrorLocal()
  {
  delete mController;
  }




QString SvMirrorLocal::controllerType() const
  {
  return mControllerType;
  }




QString SvMirrorLocal::programmName() const
  {
  //Extract signature from programm
  char str[SVVMH_SIGNATURE_LENGHT+1];
  if( SVVMH_SIGNATURE + SVVMH_SIGNATURE_LENGHT < mController->getProgramm()->codeCount() ) {
    //Код имеет достаточный размер для хранения имени программы
    for( int i = 0; i < SVVMH_SIGNATURE_LENGHT; i++ )
      str[i] = static_cast<char>( mController->getProgramm()->getCode( SVVMH_SIGNATURE + i ) );
    }
  else {
    //Программа, скорее всего, не установлена
    strcpy( str, "No programm" );
    }

  //Fill signature
  str[SVVMH_SIGNATURE_LENGHT] = 0; //Close string

  return QString::fromLatin1( str );
  }




SvVpuVector SvMirrorLocal::vpuVector() const
  {
//  int activeVpuCount = mController->vpuCount();
  int vpuMax = mController->getVpuMax();
  SvVpuVector state;
  for( int i = 0; i < vpuMax; i++ )
    state.append( *(mController->vpu(i)) );
  return state;
  }




int SvMirrorLocal::addressOfName(const QString name) const
  {
  return mController->getProgramm()->getAddr( name );
  }





int SvMirrorLocal::memoryGet(int index) const
  {
  return mController->memGet( nullptr, index );
  }





qint32 SvMirrorLocal::memoryGlobalCount() const
  {
  return mController->getProgramm()->globalCount();
  }




void SvMirrorLocal::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause, bool flash)
  {
  Q_UNUSED(flash)
  //Emit signal of start of operation [Отправим сигнал о начале операции]
  emit transferProcess( false, tr("Set programm") );

  //Setup new programm to local controller [Установить программу в исполняющую систему]
  mController->setProgrammPtr( prog );
  emit programmChanged( prog );

  //Start controller [Запускаем контроллер]
  mController->restart( runOrPause );
  if( !runOrPause )
    mScanTasks = true;


  emit transferProcess( true, QString() );

  emit linkChanged( true, controllerType(), programmName() );
  }






void SvMirrorLocal::memorySet(int index, int value)
  {
  mController->memSet( nullptr, index, value );
  }







void SvMirrorLocal::debug(int taskId, int debugCmd, int start, int stop)
  {
  mScanTasks = true;
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






//Mirror processing
void SvMirrorLocal::processing(int tickOffset)
  {
  mController->processing( tickOffset );
  if( mDivider++ >= 10 ) {
    mDivider = 0;
    //Emit memory changed
    emit memoryChanged( this );
    }

  //Проверить наличие логов
  if( mController->mLog.count() ) {
    //Логи имеются, рассылаем
    for( auto s : mController->mLog )
      emit log( s );
    // и очищаем хранилище
    mController->mLog.clear();
    }

  //Проверить наличие извещений
  if( mController->mTicket[0] ) {
    //Имеется извещение - переслать
    emit ticket( mController->mTicket[0], mController->mTicket[1], mController->mTicket[2], mController->mTicket[3],
                 mController->mTicket[4], mController->mTicket[5], mController->mTicket[6], mController->mTicket[7], mController->mTicket[8] );
    //Объявить, что извещение получено
    mController->mTicket[0] = 0;
    }

  }

