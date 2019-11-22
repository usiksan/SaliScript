/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Host-система"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Менеджер зеркал. Осущствляет переключение зеркал.

    Нам нужно, чтобы зеркало исполнялось в другом потоке.
    Для этого используем технологию worker объекта для QThread
    В качестве worker выступает зеркало. Поток постоянно циклически
    обрабатывает события в очереди, а также выполняет processing из SvMirror
*/

#ifndef SVMIRRORMANAGER_H
#define SVMIRRORMANAGER_H

#include "SvMirror.h"

#include <QObject>
#include <QThread>
#include <QMap>
#include <QElapsedTimer>
#include <QTimer>
#include <functional>

using SvMirrorFabric = std::function<SvMirrorPtr ()>;


class SvMirrorManager : public QObject
  {
    Q_OBJECT

    QMap<int,SvMirrorFabric>  mMirrorFabric; //Доступ к фабрикам зеркал
    QThread                  *mThread;       //Поток, в котором исполняется зеркало
    QTimer                   *mTimer;        //Таймер для периодического вызова функции обработки в зеркале
    QElapsedTimer            *mElapsedTimer; //Таймер для отсчета временных интервалов в зеркале
    SvMirrorPtr               mMirror;       //Активное в настоящий момент зеркало
    int                       mMirrorTypeId; //Идентификатор типа зеркала
    quint8                    padding[4];
  public:
    explicit SvMirrorManager(QObject *parent = nullptr);

    //Задача, в которой исполняется зеркало
    QThread    *mirrorThread() const { return mThread; }

    //Текущее активное зеркало
    SvMirrorPtr mirror() const { return mMirror; }

    //Идентификатор активного зеркала
    int         mirrorTypeId() const { return mMirrorTypeId; }

    //Добавить фабрику к списку
    void        addMirrorFabric( int id, SvMirrorFabric fabric );
  signals:
    void        mirrorChanged( SvMirrorPtr mirrorPtr );

  public slots:
    void        setMirror( int id );

  private slots:
    void        onStarted();
  };

#endif // SVMIRRORMANAGER_H