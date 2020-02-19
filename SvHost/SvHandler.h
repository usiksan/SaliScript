#ifndef SVHANDLER_H
#define SVHANDLER_H

#include "SvNet/SvNetChannel.h"

#include <QObject>

class SvHandler : public QObject
  {
    Q_OBJECT
  public:
    explicit SvHandler(QObject *parent = nullptr);

  signals:
    void sendBlock( SvNetChannel *ch, qint8 cmd, const QByteArray block );

    void sendAnswer(SvNetChannel *ch, qint8 srcCmd, qint32 answerCode, const QString msg );

  public slots:
    void addNetChannel( SvNetChannel *channel );

    virtual void receivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block ) = 0;

    virtual void receivedAnswer( SvNetChannel *ch, int srcCmd, qint32 answerCode, const QString msg ) = 0;

  };

#endif // SVHANDLER_H
