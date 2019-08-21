/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  Net client for communicating with remote mirror.
*/
#include "SvNetClientMirror.h"

#include <QCoreApplication>

SvNetClientMirror *svNetClientMirror;


SvNetClientMirror::SvNetClientMirror(SvMirror *mirror, int passw, int id, QString ip, int port, bool en, QObject *parent)  :
  SvNetClientFile( passw, id, ip, port, en, parent ),
  mMirror( nullptr )
  {
  setMirror( mirror );
  }




void SvNetClientMirror::setMirror(SvMirror *mirror)
  {
  //Если есть предыдущее зеркало, то отключить
  if( mMirror ) {
    disconnect( this, &SvNetClientMirror::receivedCompileFlashRun, mMirror, &SvMirror::compileFlashRun );
    disconnect( this, &SvNetClientMirror::receivedRestart, mMirror, &SvMirror::restart );
    disconnect( mMirror, &SvMirror::log, this, &SvNetClientMirror::onLog );
    }

  //Если есть новое зеркало, то подключить
  mMirror = mirror;
  if( mMirror ) {
    connect( this, &SvNetClientMirror::receivedCompileFlashRun, mMirror, &SvMirror::compileFlashRun );
    connect( this, &SvNetClientMirror::receivedRestart, mMirror, &SvMirror::restart );
    connect( mMirror, &SvMirror::log, this, &SvNetClientMirror::onLog );
    }

  }



//==========================================================================================================
//                      Подготовка запросов и размещение их в очереди

//Отправить запрос на получение директория проекта
void SvNetClientMirror::getProjectDir()
  {
  SvNetOperation op( SVC_PROJECT_DIR_GET );
  appendOperation( op );
  }




//Отправить запрос на получение переменных
void SvNetClientMirror::receivVars()
  {
  //Размещать запрос в очереди только если его там нету
  if( queueLenght() == 0 ) {
    SvNetOperation op( SVC_VAR_GET );
    appendOperation( op );
    }
  }



//Удаленная компиляция, линковка, прошивка и запуск
void SvNetClientMirror::compileFlashRun(bool link, bool flash, bool runOrPause)
  {
  SvNetOperation op( SVC_COMPILE_FLASH_RUN );
  op.mSize = 0;
  if( runOrPause ) op.mSize |= SP_RUN_OR_PAUSE;
  if( flash )      op.mSize |= SP_FLASH;
  if( link )       op.mSize |= SP_LINK;
  appendOperation( op );
  }



//Передать установленные переменные
void SvNetClientMirror::sendVars(const QByteArray array)
  {
  SvNetOperation op( SVC_VAR_SET );
  op.mArray = array;
  appendOperation( op );
  }




//Прием логического сообщения из зеркала
void SvNetClientMirror::onLog(const QString msg)
  {
  SvNetOperation op( SVC_LOG );
  op.mSrc = msg;
  appendOperation( op );
  }




//Отправить рестарт программы
void SvNetClientMirror::restart(bool run)
  {
  SvNetOperation op( SVC_RESTART );
  op.mSize = run ? 1 : 0;
  appendOperation( op );
  }




//Отправить запрос на получение информации о VPU
void SvNetClientMirror::sendVpuStateGet()
  {
  //Размещать запрос в очереди только если его там нету
  if( queueLenght() == 0 ) {
    SvNetOperation op( SVC_VPU_STATE_GET );
    appendOperation( op );
    }
  }







void SvNetClientMirror::onReadControl(SvNetPacketBlock &pk)
  {
  switch( pk.getCommand() ) {

    case SVC_COMPILE_FLASH_RUN :
      emit receivedCompileFlashRun( pk.getOffset() & SP_LINK, pk.getOffset() & SP_FLASH, pk.getOffset() & SP_RUN_OR_PAUSE );
      break;


    case SVC_PROJECT_DIR :
      //Пришел директорий проекта
      emit receivedProjectDir( pk.getString1() );
      break;

    case SVC_PROJECT_DIR_GET :
      if( mMirror != nullptr )
        pk.setString( SVC_PROJECT_DIR, mMirror->getPrjPath() );
      else
        pk.setString( SVC_PROJECT_DIR, QCoreApplication::applicationDirPath() );
      writePacket( &pk );
      break;

    case SVC_VAR_LIST :
      //Предыдущий блок данных - это переменные
      emit receivedVars( mBlock );
      break;

    case SVC_VAR_GET :
      //Запрос на получение содержимого памяти
      if( mMirror != nullptr )
        writeBlock( SVC_VAR_LIST, mMirror->memoryGlobal(), QString() );
      else
        writeBlock( SVC_VAR_LIST, QByteArray(), QString() );
      break;

    case SVC_VAR_SET :
      //Пришли переменные для установки
      if( mMirror != nullptr ) {
        QDataStream is( mBlock );
        qint32 count;
        is >> count;
        for( int i = 0; i < count; i++ ) {
          qint32 adr, val;
          is >> adr >> val;
          mMirror->memorySet( adr, val );
          }
        }
      break;


    case SVC_RESTART :
      emit receivedRestart( pk.getDataLen() );
      break;


    case SVC_LOG :
      //Оповестить о приходе лога
      emit receivedLog( pk.getString1() );
      break;

    case SVC_VPU_STATE_GET :
      //Запрос на получение состояния vpu
      if( mMirror != nullptr ) {
        //Передать текущие настройки vpu
        qint32 taskMax = mMirror->taskMax();
        qint32 taskCount = mMirror->taskCount();
        qint32 memMax    = mMirror->memorySize();
        qint32 memCount  = mMirror->memoryGlobalSize();
        QByteArray ar;
        QDataStream os( &ar, QIODevice::WriteOnly );
        os << taskMax << taskCount << memMax << memCount;
        //Записать состояние задач
        os << mMirror->taskList();
        //Записать состояние переменных
        os << mMirror->memoryGlobal();
        writeBlock( SVC_VPU_STATE, ar, QString() );
        }
      else
        //Если зеркала-источника нету, то передать пустой массив
        writeBlock( SVC_VPU_STATE, QByteArray(), QString() );
      break;

    case SVC_VPU_STATE :
      emit receivedVpuState( mBlock );
      break;


    default : SvNetClientFile::onReadControl( pk );
    }
  }




const char *SvNetClientMirror::getTitle() const
  {
  return "A.Sibilev SaliLAB SvNetClientMirror v1";
  }



void SvNetClientMirror::clearLink()
  {
  //mVarsRequest = false;
  SvNetClientFile::clearLink();
  }




bool SvNetClientMirror::operation(const SvNetOperation &op, QString &opDescr)
  {
  SvNetPacketBlock pk;
  switch( op.mCommand ) {

    case SVC_PROJECT_DIR_GET :
      pk.setCommand( SVC_PROJECT_DIR_GET );
      writePacket( &pk );
      opDescr = tr("Request project dir");
      //Требуется ожидание ответа
      return false;

    case SVC_VAR_GET :
      pk.setCommand( SVC_VAR_GET );
      writePacket( &pk );
      opDescr = tr("Request var list");
      //Требуется ожидание ответа
      return false;

    case SVC_COMPILE_FLASH_RUN :
      pk.setCommand( SVC_COMPILE_FLASH_RUN );
      //Установить команду
      pk.setOffset( op.mSize );
      //Отправить пакет
      writePacket( &pk );
      opDescr = tr("Request compile flash and run");
      break;

    case SVC_LOG :
      pk.setString( SVC_LOG, op.mSrc );
      //Отправить пакет
      writePacket( &pk );
      opDescr = tr("Log \"%1\"").arg( op.mSrc );
      break;

    case SVC_VAR_SET :
      writeBlock( SVC_VAR_SET, op.mArray, QString() );
      break;

    case SVC_VPU_STATE_GET :
      pk.setCommand( SVC_VPU_STATE_GET );
      writePacket( &pk );
      opDescr = tr("Request vpu state");
      //Требуется ожидание ответа
      return false;

    default : return SvNetClientFile::operation( op, opDescr );
    }

  return true;
  }



