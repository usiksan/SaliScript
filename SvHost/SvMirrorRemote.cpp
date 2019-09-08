/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorRemote.h"
#include "SvNetClientMirror.h"

#include <QHostAddress>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHashData>
#include <QCryptographicHash>
#include <QDataStream>

SvMirrorRemote::SvMirrorRemote(bool scanTasks) :
  SvMirrorExtern( scanTasks ),
  mRemoteId(0),    //Id удаленной машины
  mRemotePassw(0), //Passw удаленной машины
  mTime(0)
  {
  if( svNetClientMirror ) {
    connect( this, &SvMirrorRemote::linkToBridge, svNetClientMirror, &SvNetClientMirror::linkToBridge );
    connect( this, &SvMirrorRemote::linkTo, svNetClientMirror, &SvNetClientMirror::linkTo );
    connect( svNetClientMirror, &SvNetClientMirror::linkStatus, this, &SvMirrorRemote::linkStatus );
    connect( this, &SvMirrorRemote::getProjectDir, svNetClientMirror, &SvNetClientMirror::getProjectDir );
    connect( svNetClientMirror, &SvNetClientMirror::receivedProjectDir, this, [this] ( const QString dir ) {
      mRemoteDir = dir;
      });
    connect( svNetClientMirror, &SvNetClientMirror::queueOperation, this, [this] ( int count, const QString descr ) {
      setProcess( descr, count != 0 );
      });
    connect( svNetClientMirror, &SvNetClientMirror::blockTransfer, this, [this] ( int pos, int range ) {
      Q_UNUSED(range)
      setProcess( tr("Transfer %1 bytes").arg(pos) );
      });
    connect( this, &SvMirrorRemote::pushList, svNetClientMirror, &SvNetClientMirror::pushList );
    connect( this, &SvMirrorRemote::sendCompileFlashRun, svNetClientMirror, &SvNetClientMirror::compileFlashRun );
    connect( this, &SvMirrorRemote::sendVpuStateGet, svNetClientMirror, &SvNetClientMirror::sendVpuStateGet );
    connect( svNetClientMirror, &SvNetClientMirror::receivedVpuState, this, &SvMirrorRemote::receivedVpuState );
    connect( this, &SvMirrorRemote::sendVars, svNetClientMirror, &SvNetClientMirror::sendVars );
    connect( this, &SvMirrorRemote::sendRestart, svNetClientMirror, &SvNetClientMirror::restart );
    connect( svNetClientMirror, &SvNetClientMirror::receivedLog, this, &SvMirrorRemote::receivedLog );
    }
  }



SvMirrorRemote::~SvMirrorRemote()
  {

  }



//Настроить зеркало
void SvMirrorRemote::settings(const QString ip, int port, const QString globalName, const QString globalPassw, int vid, int pid)
  {
  SvMirrorExtern::settings( ip, port, globalName, globalPassw, vid, pid );

  mRemoteId    = globalName.toInt();
  mRemotePassw = globalPassw.toInt( nullptr, 16);
  //Если задано подключение
  if( !ip.isEmpty() && port ) {
    //Проверить адрес подключения удаленного управления
    if( svNetClientMirror->getIp() != ip || svNetClientMirror->getPort() != port )
      svNetClientMirror->setHost( ip, port );
    emit linkToBridge();
    }
  else if( svNetClientMirror->isLinkBridge() )
    emit linkTo( mRemoteId, mRemotePassw );
  else
    emit linkToBridge();
  }




//Выполнить обработку узла
void SvMirrorRemote::processing(int tickOffset)
  {
  //Если не задано подключение, то ничего не делаем
  if( mRemoteId == 0 )
    return;

  //Проверим наличие подключения к удаленному компьютеру
  if( !svNetClientMirror->isLink() ) {
    mTime -= tickOffset;
    //Проверим подключение к мосту
    if( !svNetClientMirror->isLinkBridge() ) {
      //К мосту еще не подключен, подключить
      if( mTime < 0 ) {
        setLink( tr("Connect to bridge"), false );
        mTime = 3000;
        emit linkToBridge();
        }
      return;
      }

    //Подключить к удаленному
    if( mTime < 0 ) {
      mTime = 3000;
      emit linkTo( mRemoteId, mRemotePassw );
      }
    return;
    }

  //Все подключено
  changeData();
  }




//Выполнить обмен с узлом
void SvMirrorRemote::changeData()
  {
  //Проверить необходимость отправки переменных
  if( mWriteQueue.count() ) {
    //Отправляем переменные
    if( mLink ) {
      //Блокиратор сдвоенного доступа к очереди
      QMutexLocker locker(&mWriteMutex);

      //Сформировать пакет на установку
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );

      qint32 count = mWriteQueue.size();
      os << count;
      for( int i = 0; i < count; i++ ) {
        qint32 adr = mWriteQueue.at(i);
        qint32 val = mWriteValues.value(adr);
        os << adr << val;
        }

      emit sendVars( ar );

      //Пакет поставлен в очередь на передачу успешно, убрать переданные переменные из массива
      mWriteQueue.clear();
      mWriteValues.clear();
      }
    }

  //Выполняем чтение переменных
  if( mLink ) {
    //Проредим запросы
    static int delay = 0;
    if( delay++ > 10 ) {
      //Отправить запрос на получение информации текущего состояния vpu
      emit sendVpuStateGet();
      delay = 0;
      }
    }
  }







void SvMirrorRemote::restart(bool runOrPause)
  {
  emit sendRestart( runOrPause );
  }





void SvMirrorRemote::compileFlashRun(bool link, bool flash, bool runOrPause)
  {
  //Если контроллер не подключен, то ничего не делаем
  if( !mLink ) {
    setProcess( tr("Error"), false, tr("Controller is not linked") );
    return;
    }

  //Отправим сигнал о начале операции
  setProcess( tr("Compile %1 in %2").arg(mMainScript).arg(mPrjPath) );

  //Выполнить компиляцию
  make();
  if( mProgramm->mErrors.count() ) {
    //При наличии ошибок прекращаем
    setProcess( tr("Complete"), false, tr("Compile errors") );
    return;
    }

  //Настроить зеркало


  //Отправить проект
  sendProject();

  setProcess( tr("Transfer programm") );
  emit sendCompileFlashRun( link, flash, runOrPause );
  }



void SvMirrorRemote::linkStatus(int id, int passw, QString bridgeName, QString linkName)
  {
  Q_UNUSED(id)
  Q_UNUSED(passw)
  if( bridgeName.isEmpty() )
    setLink( tr("Can't connect to bridge."), false );
  else {
    if( linkName.isEmpty() )
      setLink( tr("Bridge \"%1\".").arg(bridgeName), false );
    else {
      setLink( tr("Bridge \"%1\". Remote \"%2\".").arg(bridgeName).arg(linkName), true);
      //Получить удаленный директорий с проектом
      emit getProjectDir();
      }
    }
  }





//Переменные получены
void SvMirrorRemote::receivedVars(const QByteArray v)
  {
  QDataStream is( v );
  qint32 varCount;
  is >> varCount;
  bool ch = false;
  for( int i = 0; i < varCount; i++ ) {
    qint32 val;
    is >> val;
    if( i > 0 && i < mMemorySize )
      if( mMemory[i] != val ) {
        ch = true;
        mMemory[i] = val;
        }
    }

  qDebug() << "receivedVars" << varCount;

  if( ch )
    emit memoryChanged();
  }



//Получен список состояний задач
void SvMirrorRemote::receivedTasks(const QByteArray v)
  {
  QDataStream is( v );
  //Количество задач в списке
  qint32 vpuCount;
  is >> vpuCount;
  bool tc = false;
  for( int i = 0; i < vpuCount; i++ ) {
    qint32 run, ip, sp, tm, bp;
    is >> run >> ip >> sp >> tm >> bp;
    if( i < mVpuCount ) {
      if( mVpuState[i].mIsRun != run ) { mVpuState[i].mIsRun = run; tc = true; }
      mVpuState[i].mIp = ip;
      mVpuState[i].mSp = sp;
      mVpuState[i].mTm = tm;
      mVpuState[i].mBp = bp;
      }
    }

  qDebug() << "receivedTasks" << vpuCount;

  //Сообщить об изменении
  if( tc )
    emit taskChanged();
  }




//Получен лог
void SvMirrorRemote::receivedLog(const QString msg)
  {
  emit log( msg );
  }




//Получено состояние vpu
void SvMirrorRemote::receivedVpuState(const QByteArray v)
  {
  QDataStream is( v );
  //Получить текущие настройки vpu
  qint32 vpuMax;
  qint32 vpuCount;
  qint32 memMax;
  qint32 memCount;
  is >> vpuMax >> vpuCount >> memMax >> memCount;
  qDebug() << "receivedVpuState" << vpuMax << vpuCount << memMax << memCount;
  //Если настройки отличаются от текущих, то изменить
  if( vpuMax != mVpuMax || memMax != mMemorySize )
    setupMirror( vpuMax, vpuCount, memMax, memCount );

  //Прочитать состояние задач
  QByteArray ar;
  is >> ar;
  receivedTasks( ar );

  //Прочитать состояние переменных
  is >> ar;
  receivedVars( ar );
  }

