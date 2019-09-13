#ifndef SVMIRRORMANAGER_H
#define SVMIRRORMANAGER_H

#include "SvMirror.h"

#include <QObject>
#include <QThread>
#include <QMap>
#include <functional>

using SvMirrorFabric = std::function<SvMirrorPtr ()>;


class SvMirrorManager : public QObject
  {
    Q_OBJECT

    QMap<int,SvMirrorFabric>  mMirrorFabric; //Доступ к фабрикам зеркал
    QThread                  *mThread;       //Поток, в котором исполняется зеркало
    SvMirrorPtr               mMirror;       //Активное в настоящий момент зеркало
    int                       mMirrorTypeId; //Идентификатор типа зеркала

    //Mirror settings
    int                       mPort;
    QString                   mIp;
    QString                   mGlobalName;
    QString                   mGlobalPassw;
    int                       mVid;
    int                       mPid;
  public:
    explicit SvMirrorManager(QObject *parent = nullptr);

    //Текущее активное зеркало
    SvMirrorPtr mirror() const { return mMirror; }

    //Добавить фабрику к списку
    void        addMirrorFabric( int id, SvMirrorPtr theMirror );
  signals:

  public slots:
    void setMirror( int id );

    void setSettings( const QString ip, int port,
                      const QString globalName, const QString globalPassw,
                      int vid, int pid );
  };

#endif // SVMIRRORMANAGER_H
