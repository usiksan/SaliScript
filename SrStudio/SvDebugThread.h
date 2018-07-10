/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Задача исполнения отладочного потока
*/

#ifndef SVDEBUGTHREAD_H
#define SVDEBUGTHREAD_H

#include <QThread>
#include <QElapsedTimer>

#include "Host/SvMirror.h"

class SvDebugThread : public QThread
  {
    Q_OBJECT

    QElapsedTimer mTimer;
    qint64        mPrevTimer;
  public:
    SvDebugThread( QObject *parent = 0 );
    bool mUseNewLibusb;
    ~SvDebugThread();

    //Текущий клиент отладки
    static SvMirror *mClient;

    //Поток отладки
    static SvDebugThread *mThread;

  signals:
    void debugChanged();

  public slots:
    //Установить новый режим отладки
    void setDebugMode( int mode );

    //Синхронизировать режим отладки с проектом
    void syncDebugMode();

    void setDebugSimulate() { setDebugMode( SMT_LOCAL ); }
    void setDebugUsb()      { setDebugMode( SMT_USB ); }
    void setDebugRemote()   { setDebugMode( SMT_REMOTE ); }

    //Периодическая функция
    void periodic();

    // QThread interface
  protected:
    virtual void run();
  };


#endif // SVDEBUGTHREAD_H
