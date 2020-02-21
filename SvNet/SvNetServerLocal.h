#ifndef SVNETSERVERLOCAL_H
#define SVNETSERVERLOCAL_H


#include "SvNetChannel.h"

#include <QTcpServer>

class SvNetServerLocal : public QTcpServer
  {
    Q_OBJECT
  public:
    SvNetServerLocal( int port );

  signals:
    void addNetChannel( SvNetChannel *channel );
  };

#endif // SVNETSERVERLOCAL_H
