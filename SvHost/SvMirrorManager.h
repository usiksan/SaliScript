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

    QThread                  *mThread;       //Поток, в котором исполняется зеркало
    QTimer                   *mTimer;        //Таймер для периодического вызова функции обработки в зеркале
    QElapsedTimer            *mElapsedTimer; //Таймер для отсчета временных интервалов в зеркале
    QMap<int,SvMirrorFabric>  mMirrorFabric; //Доступ к фабрикам зеркал
    SvMirrorPtr               mMirror;       //Активное в настоящий момент зеркало
    int                       mMirrorTypeId; //Идентификатор типа зеркала
    bool                      mNeedInit;     //! After mirror creation it placed into the execution thread mThread.
                                             //! Before start any operations mirror must be initialised from inside thread.
                                             //! This flag used to call special init method of mirror to perform this job.
    quint8                    padding[3];
  public:
    explicit SvMirrorManager(QObject *parent = nullptr);

    //Задача, в которой исполняется зеркало
    QThread    *mirrorThread() const { return mThread; }

    //Текущее активное зеркало
    SvMirrorPtr mirror() const { return mMirror; }

    //Идентификатор активного зеркала
    int         mirrorTypeId() const { return mMirrorTypeId; }

    //!
    //! \brief addMirrorFabric Добавить фабрику к списку. Фабрика - это функтор, используемый для конструирования зеркала.
    //!                        Фабрика вызывается в контексте потока-обработчика зеркал.
    //! \param id              Идентификатор данной фабрики
    //! \param fabric          Собственно фабрика
    //!
    void        addMirrorFabric( int id, SvMirrorFabric fabric );
  signals:
    //!
    //! \brief mirrorChanged Сигнал посылается при изменении активного зеркала, т.е.при переключении
    //! \param id            Идентификатор зеркала
    //! \param mirrorPtr     Указатель на новое активное зеркало
    //!
    void        mirrorChanged( int id, SvMirrorPtr mirrorPtr );

  public slots:
    void        setMirror(SvMirrorPtr newMirror, int id);
    //!
    //! \brief setMirrorById Используется для смены активного зеркала. На основе идентификатора зеркала
    //!                      строится новое зеркало и делается текущим. Старое зеркало ставится на удаление
    //! \param id            Идентификатор зеркала, которое необходимо установить
    //!
    void        setMirrorById( int id );


    void        stop();

  protected slots:
    //!
    //! \brief start This slot atomatic called on manager thread creation
    //!
    void        start();

    void        periodic();
  };

#endif // SVMIRRORMANAGER_H
