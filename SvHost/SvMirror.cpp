/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirror.h"
#include "SvMirrorThread.h"
#include "Compiler/SvVpuCompiler.h"

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
void SvMirror::settings(const QString prjPath, const QString mainScript, const QString ip, int port, const QString globalName, const QString globalPassw, int vid, int pid)
  {
  mPrjPath    = prjPath;
  mMainScript = mainScript;
  //mName       = name;
  Q_UNUSED(ip)
  Q_UNUSED(port)
  Q_UNUSED(globalName)
  Q_UNUSED(globalPassw)
  Q_UNUSED(vid)
  Q_UNUSED(pid)
  }





//Получить все задачи списком
QByteArray SvMirror::taskList()
  {
  QByteArray dst;
  QDataStream os( &dst, QIODevice::WriteOnly );
  qint32 count = taskCount();
  os << count;
  for( int i = 0; i < count; i++ ) {
    qint32 run, ip, sp, tm, bp;
    taskInfo( i, &run, &ip, &sp, &tm, &bp );
    os << run << ip << sp << tm << bp;
    }
  return dst;
  }





QByteArray SvMirror::memory()
  {
  QByteArray dst;
  QDataStream os( &dst, QIODevice::WriteOnly );
  qint32 count = memorySize();
  os << count;
  qint32 val;
  for( int i = 0; i < count; i++ ) {
    val = memoryGet(i);
    os << val;
    }
  return dst;
  }




//Получить размер глобальной памяти
int SvMirror::memoryGlobalSize() const
  {
  return qMin( mProgramm->globalCount(), memorySize() );
  }




//Получить только глобальную память
QByteArray SvMirror::memoryGlobal()
  {
  QByteArray dst;
  QDataStream os( &dst, QIODevice::WriteOnly );
  qint32 count = memoryGlobalSize();
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
  for( int i = 0; i < taskCount(); i++ )
    debugRun( i );
  }





//Отладка - стоп (пауза)
void SvMirror::debugPause(int taskId)
  {
  debugRunUntil( taskId, 0, mProgramm->codeCount() );
  }





//Отладка - стоп (пауза) всех задач
void SvMirror::debugPauseAll()
  {
  for( int i = 0; i < taskCount(); i++ )
    debugPause( i );
  }





void SvMirror::debugStep(int taskId)
  {
  int ip, run;
  if( taskInfo( taskId, &run, &ip, 0, 0, 0 ) ) {
    //Задача присутствует
    if( run == 0 && ip != 0 ) {
      //Задача заторможена
      int line = mProgramm->getLine(ip);
      int file = mProgramm->getFile(ip);
      //Пропустить текущую строку
      int ipe = ip+1;
      while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
      debugRunStep( taskId, ip, ipe );
      }
    else debugPause( taskId );
    }
  }




void SvMirror::debugTrace(int taskId)
  {
  int ip, run;
  if( taskInfo( taskId, &run, &ip, 0, 0, 0 ) ) {
    //Задача присутствует
    if( run == 0 && ip != 0 ) {
      //Задача заторможена
      int line = mProgramm->getLine(ip);
      int file = mProgramm->getFile(ip);
      //Пропустить текущую строку
      int ipe = ip+1;
      while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
      debugRunTrace( taskId, ip, ipe );
      }
    else debugPause( taskId );
    }
  }



//Завершить вызов удаленной процедуры и вернуть результат
void SvMirror::remoteCallComplete(int result)
  {
  Q_UNUSED(result)
  }



//Запустить скрипт на исполнение
//scriptPath - полный путь к скрипту
void SvMirror::startScript(const QString scriptPath)
  {
  QFileInfo info(scriptPath);

  //Записать путь к проекту
  mPrjPath = info.absolutePath();
  mPrjPath.append( QChar('/') );

  //Записать имя скрипта
  mMainScript = info.fileName();

  //Запустить исполнение
  compileFlashRun( false, true, true );
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





//Построить программу
void SvMirror::make()
  {
  if( mMainScript.endsWith( SV_PROGRAMM_EXTENSION ) ) {
    //Нам передан бинарник программы, просто загружаем
    mProgramm->load( mPrjPath + mMainScript );
    }
  else {
    SvVpuCompiler compiler;
    //Построить программу
    mProgramm = compiler.make( mPrjPath, mMainScript );
    //Сохранить бинарник
    //Заменить расширение
    auto i = mMainScript.lastIndexOf( QChar('.') );
    QString prg;
    if( i >= 0 )
      prg = mMainScript.left(i) + SV_PROGRAMM_EXTENSION;
    else
      prg = mMainScript + SV_PROGRAMM_EXTENSION;
    mProgramm->save( mPrjPath + prg );
    }
  }













