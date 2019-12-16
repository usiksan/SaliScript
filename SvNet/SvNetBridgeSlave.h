#ifndef SVNETBRIDGESLAVE_H
#define SVNETBRIDGESLAVE_H

#include "SvNetChannel.h"
#include "SvNetService.h"

#include <QObject>

class SvNetBridgeSlave : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetBridgeSlave(QObject *parent = nullptr);

    void addService( SvNetService *service );
  signals:
    void linkStatusChanged( bool linked, const QString msg );

  public slots:
    //void setup( const QString bridgeIp, const QString id, const QString remotePassw );
  };

#endif // SVNETBRIDGESLAVE_H
