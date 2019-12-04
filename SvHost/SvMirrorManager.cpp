#include "SvMirrorManager.h"
#include "SvMirrorLocal.h"
#include "SvVMachineLocal.h"

SvMirrorManager::SvMirrorManager(QObject *parent) :
  QObject(parent),
  mThread(nullptr),
  mTimer(nullptr),
  mElapsedTimer(nullptr),
  mMirror(nullptr),
  mMirrorTypeId(-1)
  {
  mThread = new QThread();
  moveToThread( mThread );
  connect( mThread, &QThread::started, this, &SvMirrorManager::start );
  mThread->start();
  }



void SvMirrorManager::addMirrorFabric(int id, SvMirrorFabric fabric)
  {
  mMirrorFabric.insert( id, fabric );
  }



void SvMirrorManager::setMirror(int id)
  {
  if( id != mMirrorTypeId && mMirrorFabric.contains(id) ) {
    //If present previous mirror then delete it
    if( mMirror ) mMirror->deleteLater();
    //Build new mirror
    mMirror = mMirrorFabric.value(id)();
    mMirrorTypeId = id;
    //Move it to thread
    mMirror->moveToThread( mThread );
    emit mirrorChanged( id, mMirror );
    }
  }




void SvMirrorManager::stop()
  {
  if( mMirror )
    mMirror->deleteLater();
  if( mElapsedTimer )
    delete mElapsedTimer;
  if( mTimer )
    delete mTimer;
  mThread->quit();
  mThread->wait();
  }




void SvMirrorManager::start()
  {
  mElapsedTimer = new QElapsedTimer();
  mTimer = new QTimer();
  connect( mTimer, &QTimer::timeout, this, &SvMirrorManager::periodic );
  mTimer->start( 10 );
  mElapsedTimer->start();
  if( mMirrorFabric.count() == 0 )
    addMirrorFabric( 0, [] () -> SvMirrorPtr { return new SvMirrorLocal( new SvVMachineLocal(), false ); } );
  setMirror( mMirrorFabric.firstKey() );
  }




void SvMirrorManager::periodic()
  {
  if( mMirror )
    mMirror->processing( static_cast<int>(mElapsedTimer->restart()) );
  }
