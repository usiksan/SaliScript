/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirror.h"
#include "SvMirrorThread.h"
#include "SvCompiler/SvVpuCompiler.h"
#include "SvProgramm.h"

#include <QFileInfo>


SvMirror::SvMirror(bool scanTasks ) :
  mProgramm( new SvProgramm() ),
  mLink(false),
  mProcess(false),
  mScanTasks(scanTasks)
  {
  mLinkStatus = tr("Unconnected");
  }

SvMirror::~SvMirror()
  {

  }




//Настроить зеркало
void SvMirror::settings(const QString ip, int port, const QString globalName, const QString globalPassw, int vid, int pid)
  {
  Q_UNUSED(ip)
  Q_UNUSED(port)
  Q_UNUSED(globalName)
  Q_UNUSED(globalPassw)
  Q_UNUSED(vid)
  Q_UNUSED(pid)
  }





void SvMirror::proceccingRemoteCall()
  {
  //Проверить необходимость вызова удаленной процедуры
  if( memoryGetByName(QString("remoteCallId")) != mRemoteCallId ) {
    //Нужен вызов
    //Фиксируем номер процедуры для исключения повторного вызова одной и той-же процедуры
    mRemoteCallId = memoryGetByName(QString("remoteCallId"));
    //Получаем адреса возврата результата
    mRemoteAddr0 = memoryGetByName(QString("remoteResult0"));
    mRemoteAddr1 = memoryGetByName(QString("remoteResult1"));
    mRemoteAddr2 = memoryGetByName(QString("remoteResult2"));
    mRemoteAddr3 = memoryGetByName(QString("remoteResult3"));
    //qDebug() << "proceccingRemoteCall" << mRemoteCallId << mRemoteAddr0 << mRemoteAddr1 << mRemoteAddr2 << mRemoteAddr3;
    if( mRemoteCallId )
      emit remoteCall( mRemoteCallId, memoryGetByName(QString("remoteParam0")), memoryGetByName(QString("remoteParam1")),
                       memoryGetByName(QString("remoteParam2")), memoryGetByName(QString("remoteParam3")) );
    }
  }





//Получить все задачи списком
QByteArray SvMirror::taskList()
  {
  QByteArray dst;
  QDataStream os( &dst, QIODevice::WriteOnly );
  qint32 count = mControllerInfo.mVpuCount;
  os << count;
  for( int i = 0; i < count; i++ ) {
    os << taskInfo( i );
    }
  return dst;
  }





//QByteArray SvMirror::memory()
//  {
//  QByteArray dst;
//  QDataStream os( &dst, QIODevice::WriteOnly );
//  qint32 count = memorySize();
//  os << count;
//  qint32 val;
//  for( int i = 0; i < count; i++ ) {
//    val = memoryGet(i);
//    os << val;
//    }
//  return dst;
//  }




//Получить размер глобальной памяти
//int SvMirror::memoryGlobalSize() const
//  {
//  return qMin( mProgramm->globalCount(), memorySize() );
//  }




//Получить только глобальную память
QByteArray SvMirror::memoryGlobal()
  {
  QByteArray dst;
  QDataStream os( &dst, QIODevice::WriteOnly );
  qint32 count = mProgramm->globalCount();
  os << count;
  qint32 val;
  for( int i = 0; i < count; i++ ) {
    val = memoryGet(i);
    os << val;
    }
  return dst;
  }




//Получить адрес символа
int SvMirror::addressOfName(const QString &name) const
  {
  return mProgramm->getAddr( name );
  }




//Отладка - пуск всех задач
void SvMirror::debugRunAll()
  {
  for( int i = 0; i < mControllerInfo.mVpuCount; i++ )
    debug( i, SDC_RUN, 0, 0 );
  }





//Отладка - стоп (пауза)
void SvMirror::debugPause(int taskId)
  {
  debug( taskId, SDC_RUN_UNTIL, 0, mProgramm->codeCount() );
  }





//Отладка - стоп (пауза) всех задач
void SvMirror::debugPauseAll()
  {
  for( int i = 0; i < mControllerInfo.mVpuCount; i++ )
    debugPause( i );
  }





void SvMirror::debugStep(int taskId)
  {
  if( taskId >= 0 && taskId < mControllerInfo.mVpuCount ) {
    //Задача присутствует
    SvVmVpuState st = taskInfo(taskId);
    if( !st.mDebugRun && st.mIp != 0 ) {
      //Задача заторможена
      int line = mProgramm->getLine(st.mIp);
      int file = mProgramm->getFile(st.mIp);
      //Пропустить текущую строку
      int ipe = st.mIp + 1;
      while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
      debug( taskId, SDC_RUN_STEP, st.mIp, ipe );
      }
    else debugPause( taskId );
    }
  }




void SvMirror::debugTrace(int taskId)
  {
  if( taskId >= 0 && taskId < mControllerInfo.mVpuCount ) {
    //Задача присутствует
    SvVmVpuState st = taskInfo(taskId);
    if( !st.mDebugRun && st.mIp != 0 ) {
      //Задача заторможена
      int line = mProgramm->getLine(st.mIp);
      int file = mProgramm->getFile(st.mIp);
      //Пропустить текущую строку
      int ipe = st.mIp + 1;
      while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
      debug( taskId, SDC_RUN_TRACE, st.mIp, ipe );
      }
    else debugPause( taskId );
    }
  }






//Периодическая функция, обеспечивающая работу зеркала
void SvMirror::periodic()
  {
  processing( static_cast<int>(mTimer.restart()) );
  }








void SvMirror::remoteCallComplete(int r0, int r1, int r2, int r3)
  {
  //Установить результат
  if( mRemoteAddr0 ) memorySet( mRemoteAddr0, r0 );
  if( mRemoteAddr1 ) memorySet( mRemoteAddr1, r1 );
  if( mRemoteAddr2 ) memorySet( mRemoteAddr2, r2 );
  if( mRemoteAddr3 ) memorySet( mRemoteAddr3, r3 );
  //Сбросить флаг активного вызова
  mRemoteCallId = 0;
  memorySetByName( QString("remoteCallId"), 0 );
  }






void SvMirror::compileFlashRun(const QString scriptPath, bool link, bool flash, bool runOrPause)
  {
  QFileInfo info(scriptPath);

  //Записать путь к проекту
  QString prjPath = info.absolutePath();
  prjPath.append( QChar('/') );

  //Записать имя скрипта
  QString mainScript = info.fileName();

  if( mainScript.endsWith( SV_PROGRAMM_EXTENSION ) ) {
    //Нам передан бинарник программы, просто загружаем
    mProgramm->load( prjPath + mainScript );
    }
  else {
    //Построить программу
    //Бинарник сохраняется автоматически при построении
    mProgramm = SvCompiler6::SvVpuCompiler::build( prjPath, mainScript );
    }
  setProgrammFlashRun( mProgramm, link, flash, runOrPause );
  }




//Запустить скрипт на исполнение
//scriptPath - полный путь к скрипту
void SvMirror::startScript(const QString scriptPath)
  {
  //Запустить исполнение
  compileFlashRun( scriptPath, false, true, true );
  }





//Установить новое состояние связи
void SvMirror::setLink(const QString status, bool lnk)
  {
  //Переменная для определения изменений в состоянии
  bool update = false;

  //Проверить, изменилось ли текстовое состояние
  if( mLinkStatus != status ) {
    //Текстовое состояние изменилось - обновить
    mLinkStatus = status;
    update = true;
    }

  //Проверить, изменилось ли состояние подключения
  if( mLink != lnk ) {
    mLink = lnk;
    update = true;
    }

  if( update )
    emit linkStatusChanged( mLink, mLinkStatus );
  }










//Установить новый процесс операции
void SvMirror::setProcess( const QString status, bool processStatus, const QString error )
  {
  mProcess       = processStatus;
  mProcessStatus = status;
  mProcessError  = error;
  emit processChanged( mProcessStatus, mProcess, mProcessError );
  }

















