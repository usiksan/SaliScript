/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorLocal.h"

//#include <QDebug>

SvMirrorLocal::SvMirrorLocal(SvVMachineLocal *controller ) :
  SvMirror(),
  mController(controller),
  mDivider(0),
  mVpuDebugRun( mController->getVpuMax(), 0 )
  {

  }



SvMirrorLocal::~SvMirrorLocal()
  {
  delete mController;
  }




int SvMirrorLocal::addressOfName(const QString name) const
  {
  return mController->getProgramm()->getAddr( name );
  }





int SvMirrorLocal::memoryGet(int index) const
  {
  return mController->memGet( nullptr, index );
  }




//Установка программы, прошивка и запуск
void SvMirrorLocal::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause)
  {
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

  //Extract signature from programm
  char str[SVVMH_SIGNATURE_LENGHT+1];
  for( int i = 0; i < SVVMH_SIGNATURE_LENGHT; i++ )
    str[i] = static_cast<char>( prog->getCode( SVVMH_SIGNATURE + i ) );

  //Fill signature
  str[SVVMH_SIGNATURE_LENGHT] = 0; //Close string
  emit linkChanged( true, tr("Local controller"), QString::fromLatin1( str ) );
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
    //Emit task changed
    if( mScanTasks ) {
      for( int i = 0; i < mController->getVpuMax(); i++ ) {
        const SvVmVpu *vpu = mController->vpu(i);
        if( mVpuDebugRun[i] || vpu->mDebugRun ) {
          mVpuDebugRun[i] = vpu->mDebugRun;
          emit taskChanged( i, vpu->mIp, vpu->mSp, vpu->mBp, vpu->mTm, vpu->mBaseSp, vpu->mThrow, vpu->mDebugRun );
          }
        }
      }
    }

  }

