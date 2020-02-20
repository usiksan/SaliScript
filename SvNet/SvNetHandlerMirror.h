#ifndef SVNETHANDLERMIRROR_H
#define SVNETHANDLERMIRROR_H

#include "SvNetHandler.h"
#include "SvHost/SvMirror.h"

#include <QMap>
#include <QList>



class SvNetHandlerMirror : public SvNetHandler
  {
    Q_OBJECT

    SvMirrorPtr mMirror;
    QStringList mLogList;
  public:
    SvNetHandlerMirror();

  public slots:
    void mirrorChanged( int id, SvMirrorPtr mirrorPtr );

    // SvNetHandler interface
  public slots:
    virtual void receivedBlock(SvNetChannel *ch, qint8 cmd, QByteArray block) override;
  };

#endif // SVNETHANDLERMIRROR_H
