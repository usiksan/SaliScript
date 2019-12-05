#ifndef SVNETSERVER_H
#define SVNETSERVER_H

#include <QTcpServer>

class SvNetServer : public QTcpServer
  {
    Q_OBJECT

  public:
    SvNetServer();
  };

#endif // SVNETSERVER_H
