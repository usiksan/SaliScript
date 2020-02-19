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

    void sendAnswer(SvNetChannel *ch, qint8 srcCmd, qint32 answerCode, const QString msg );

  public slots:
    void addNetChannel( SvNetChannel *channel );

    virtual void receivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block ) = 0;

    virtual void receivedAnswer( SvNetChannel *ch, int srcCmd, qint32 answerCode, const QString msg ) = 0;
  };

#endif // SVNETHANDLER_H
