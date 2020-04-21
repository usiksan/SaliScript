#include "SvMirrorManager.h"
#include "SvMirrorLocal.h"
#include "SvVMachineLocal.h"

SvMirrorManager::SvMirrorManager(QObject *parent) :
  QObject(parent),
  mThread(nullptr),
  mTimer(nullptr),
  mElapsedTimer(nullptr),
  mMirror(nullptr),
  mMirrorTypeId(-1),
  mNeedInit(true)
  {
  mThread = new QThread();
  moveToThread( mThread );
  connect( mThread, &QThread::started, this, &SvMirrorManager::start );
  }



//!
//! \brief threadStart Запустить поток с обслуживанием зеркал
//!
void SvMirrorManager::threadStart()
  {
  mThread->start();
  }



void SvMirrorManager::addMirrorFabric(int id, SvMirrorFabric fabric)
  {
  mMirrorFabric.insert( id, fabric );
  }




void SvMirrorManager::setMirror(SvMirrorPtr newMirror, int id)
  {
  //If present previous mirror then delete it
  if( mMirror ) mMirror->deleteLater();
  mMirror = newMirror;
  mMirrorTypeId = id;
  mNeedInit = true;
  //Move it to thread
  mMirror->moveToThread( mThread );
  emit mirrorChanged( id, mMirror );
  }



void SvMirrorManager::setMirrorById(int id)
  {
  if( id != mMirrorTypeId && mMirrorFabric.contains(id) ) {
    //Build new mirror and setup
    setMirror( mMirrorFabric.value(id)(), id );
    }
//  else
//    emit mirrorChanged( id, mMirror );
  }




void SvMirrorManager::stop()
  {
  if( mTimer ) {
    //Остановить таймер
    mTimer->stop();
    delete mTimer;
    }
  //If present previous mirror then delete it
  if( mMirror ) {
    mMirror->deleteLater();
    mMirror.clear();
    }
  if( mElapsedTimer )
    delete mElapsedTimer;
  mThread->quit();
  mThread->wait();
  }




void SvMirrorManager::start()
  {
  //Create timers
  mTimer = new QTimer();
  mElapsedTimer = new QElapsedTimer();

  //Connect to timer handler
  //This handler periodicaly executes method process of current active mirror
  connect( mTimer, &QTimer::timeout, this, &SvMirrorManager::periodic );
  mTimer->start(10);
  mElapsedTimer->start();

  //Here we create default mirror
  //If no mirror creators present in fabric map then we create default fabric with local mirror
  if( mMirrorFabric.count() == 0 )
    //Default mirror fabric with local mirror
    addMirrorFabric( 0, [] () -> SvMirrorPtr { return SvMirrorPtr(new SvMirrorLocal( new SvVMachineLocal() ) ); } );

  //Start first available mirror
  setMirrorById( mMirrorFabric.firstKey() );
  }




void SvMirrorManager::periodic()
  {
  //Periodicaly execute mirror handle only if mirror present
  if( !mMirror.isNull() ) {

    //Flag mNeedInit provides one-time call of init method of mirror after mirror
    // activated
    if( mNeedInit ) {
      mirror()->init();
      mNeedInit = false;
      }

    //Periodicaly execute processing method
    mMirror->processing( static_cast<int>(mElapsedTimer->restart()) );
    }
  }
