/*
  Проект "QML интерфейс к svpu"
  Автор
    Сибилев А.С.
  Описание
    Исполнитель зеркала

    Нам нужно, чтобы зеркало исполнялось в другом потоке.
    Для этого используем технологию worker объекта для QThread
    В качестве worker выступает зеркало. Поток постоянно циклически
    обрабатывает события в очереди, а также выполняет processing из SvMirror
  */
#ifndef SVMIRRORTHREAD_H
#define SVMIRRORTHREAD_H

#include "SvMirror.h"

#include <QThread>
#include <QElapsedTimer>

class SvMirrorThread : public QThread
  {
    Q_OBJECT

    SvMirror      *mMirror;     //Зеркало, которое работает в данной QThread
    QElapsedTimer  mTimer;      //Для отсчета временных интервалов
    qint64         mPrevTimer;  //Механизм для определения относительного времени
  public:
    //Используется уже готовое зеркало
    SvMirrorThread( SvMirror *mirror );

    ~SvMirrorThread() override;

    //Вернуть зеркало, исполняемое в данной QThread
    SvMirror* getMirror() const { return mMirror; }

  public slots:
    //Периодическая функция, обеспечивающая работу зеркала
    void periodic();

  // QThread interface
  protected:
    virtual void run() override;
  };

#endif // SVMIRRORTHREAD_H
