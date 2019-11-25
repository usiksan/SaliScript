/*
  Project "Net packet interchange over local net and internet bridge server"
  Автор
    Sibilev A.S.

    www.salilab.com
    www.salilab.ru
  Description
    The service interacts with the channel to perform a specific exchange command group
*/
#ifndef SVNETSERVICE_H
#define SVNETSERVICE_H

#include "SvNetChannel.h"

#include <QObject>

class SvNetService : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetService(QObject *parent = nullptr);

  signals:
    void sendBlock( SvNetChannel *ch, qint8 cmd, const QByteArray block );

  public slots:
    virtual void onReceivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block );
  };

#endif // SVNETSERVICE_H
