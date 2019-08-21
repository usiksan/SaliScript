/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Поток выполнения отладки
*/
#include <QTimer>
#include "SvDebugThread.h"
#include "SvProject.h"
#include "Host/SvMirrorLocal.h"
#include "Host/SvMirrorUsb10.h"
#include "Host/SvMirrorRemote.h"

//Текущий клиент отладки
SvMirror *SvDebugThread::mClient = nullptr;


//Поток отладки
SvDebugThread *SvDebugThread::mThread;



SvDebugThread::SvDebugThread(QObject *parent) :
  QThread(parent)
  {
  //Переносим объект потока в данный поток
  moveToThread( this );

  mThread = this;
  mClient = nullptr;
  }




SvDebugThread::~SvDebugThread()
  {
  if( mClient )
    delete mClient;
  }



//Установить новый режим отладки
void SvDebugThread::setDebugMode(int mode)
  {
  //Запомнить в проекте новый тип отладки
  svProject->mDebugType = mode;

  //Синхронизировать фактический тип отладки с проектом
  syncDebugMode();
  }




void SvDebugThread::syncDebugMode()
  {
  qDebug() << "syncDebug";
  //Проверить если текущий отладчик совпадает с заданным, то ничего не делаем
  if( mClient == nullptr || mClient->mirrorType() != svProject->mDebugType ) {

    //Удалить прежний отладчик, если есть
    if( mClient ) {
      //Удалить все связи отладчика
      mClient->disconnect();
      //Удалить сам отладчик
      mClient->deleteLater();
      mClient = nullptr;
      }

    //Создать новый отладчик
    SvMirror *cl = nullptr;
    switch( svProject->mDebugType ) {
      case SMT_UNDEFINED :
      case SMT_LOCAL :
      default :
        cl = new SvMirrorLocal( new SvControllerLocal(), true );
        //Обеспечить возможность удаленного подключения
        if( svNetClientMirror != nullptr )
          svNetClientMirror->setMirror( cl );
        break;

      case SMT_USB :
        cl = new SvMirrorUsb10( true );
        //Обеспечить возможность удаленного подключения
        if( svNetClientMirror != nullptr )
          svNetClientMirror->setMirror( cl );
        break;

      case SMT_REMOTE :
        cl = new SvMirrorRemote( true );
        //Для удаленного зеркала удаленное подключение не разрешаем
        if( svNetClientMirror != nullptr )
          svNetClientMirror->setMirror( nullptr );
        break;

      }

    cl->moveToThread( this );
    mClient = cl;
    }

  //Обновить показатели проекта
  mClient->settings( svProject->mProjectPath, svProject->mMainScript,
                     svProject->mRemoteIp, svProject->mRemotePort,
                     svProject->mGlobalId, svProject->mPassw,
                     svProject->mVid, svProject->mPid );

  emit debugChanged();
  }




//Периодическая функция
void SvDebugThread::periodic()
  {
  qint64 t = mTimer.elapsed();
  if( mClient )
    mClient->processing( (int)(t - mPrevTimer) );
  mPrevTimer = t;
  }





void SvDebugThread::run()
  {
  //Создаем отладчик по умолчанию
  syncDebugMode();

  //Создать таймер
  QTimer timer;

  //Подключить таймер к периодической функции обработчика
  connect( &timer, SIGNAL(timeout()), this, SLOT(periodic()) );
  //Старт таймера
  timer.start(10);
  mTimer.start();

  //Запустить цикл обработки сообщений
  exec();
  }

