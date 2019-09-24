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
    Доступ к зеркалу по локальной сети
*/
#ifndef SVNETMIRRORLOCAL_H
#define SVNETMIRRORLOCAL_H

#include "SvMirror.h"

#include <QObject>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

class SvNetMirrorLocal : public QObject
  {
    Q_OBJECT

    QTcpServer *mServer;
    QTcpSocket *mSocket;
  public:
    explicit SvNetMirrorLocal(QObject *parent = nullptr);

  signals:

  public slots:
    void setMirror( SvMirror *mirror );
  };

#endif // SVNETMIRRORLOCAL_H
