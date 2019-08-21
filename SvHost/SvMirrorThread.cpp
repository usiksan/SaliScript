/*
  Проект "QML интерфейс к svpu"
  Автор
    Сибилев А.С.
  Описание
    Исполнитель зеркала
  */
#include "SvMirrorThread.h"

#include <QTimer>
#include <QAbstractEventDispatcher>

//Используется уже готовое зеркало
SvMirrorThread::SvMirrorThread(SvMirror *mirror)
  {
  mMirror = mirror;

  //Переместить зеркало в данный поток
  mMirror->moveToThread( this );
  }




SvMirrorThread::~SvMirrorThread()
  {
  if( mMirror )
    delete mMirror;
  }




//Периодическая функция, обеспечивающая работу зеркала
void SvMirrorThread::periodic()
  {
  qint64 t = mTimer.elapsed();
  if( mMirror )
    mMirror->processing( (int)(t - mPrevTimer) );
  mPrevTimer = t;
  }





//Исполняется в другом потоке и обрабатывает события, а
// также исполняет периодические операции
void SvMirrorThread::run()
  {
  //Создать таймер
  QTimer timer;

  //Подключить таймер к периодической функции обработчика
  connect( &timer, SIGNAL(timeout()), this, SLOT(periodic()) );
  //Старт таймера
  timer.start(10);
  mTimer.start();
  mPrevTimer = mTimer.elapsed();

  //Запустить цикл обработки сообщений
  exec();
  }

