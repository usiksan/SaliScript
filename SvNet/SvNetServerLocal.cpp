#include "SvNetServerLocal.h"
#include "SvNetChannel.h"

SvNetServerLocal::SvNetServerLocal(int port) :
  QTcpServer()
  {
  listen( QHostAddress::Any, port );
  }
