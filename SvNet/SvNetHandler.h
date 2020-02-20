#ifndef SVNETHANDLER_H
#define SVNETHANDLER_H

#include "SvNetChannel.h"

#include <QObject>
#include <QDataStream>
#include <QByteArray>

class SvNetHandler : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetHandler(QObject *parent = nullptr);

  signals:
    void sendBlock( SvNetChannel *ch, qint8 cmd, const QByteArray block );


  public slots:
    void addNetChannel( SvNetChannel *channel );

    virtual void receivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block ) = 0;
  };

#endif // SVNETHANDLER_H
