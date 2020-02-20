/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirror.h"
#include "SvCompiler/SvVpuCompiler.h"
#include "SvProgramm.h"

#include <QFileInfo>
#include <QTimer>


SvMirror::SvMirror() :
  QObject(),
  mScanTasks(false)
  {
  }



SvMirror::~SvMirror()
  {

  }

void SvMirror::linkTo(const QString ipOrUsb, int port, const QString controllerName, const QString controllerPassw)
  {
  //There do nothing
  Q_UNUSED(ipOrUsb)
  Q_UNUSED(port)
  Q_UNUSED(controllerName)
  Q_UNUSED(controllerPassw)
  }




void SvMirror::compileFlashRun(const QString scriptPath, bool runOrPause)
  {
  QFileInfo info(scriptPath);

  //Записать путь к проекту
  QString prjPath = info.absolutePath();
  prjPath.append( QChar('/') );

  //Записать имя скрипта
  QString mainScript = info.fileName();

  SvProgrammPtr prog( new SvProgramm{} );

  if( mainScript.endsWith( SV_PROGRAMM_EXTENSION ) ) {
    //Нам передан бинарник программы, просто загружаем
    prog->load( prjPath + mainScript );
    }
  else {
    //Построить программу
    //Бинарник сохраняется автоматически при построении
    prog = SvCompiler6::SvVpuCompiler::build( prjPath, mainScript );
    }
  setProgrammFlashRun( prog, runOrPause, true );
  }




//Запустить скрипт на исполнение
//scriptPath - полный путь к скрипту
void SvMirror::startScript(const QString scriptPath)
  {
  //Запустить исполнение
  compileFlashRun( scriptPath, true );
  }



#if 0

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
  qint32 count = taskCount();
  os << count;
  SvVmVpuState tskInfo;
  for( int i = 0; i < count; i++ ) {
    taskInfo( i, tskInfo );
    os << tskInfo;
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


#endif
