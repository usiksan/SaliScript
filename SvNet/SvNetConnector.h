#ifndef SVNETCONNECTOR_H
#define SVNETCONNECTOR_H

#include "SvNetChannel.h"

#include <QObject>

class SvNetConnector : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetConnector(QObject *parent = nullptr);

  signals:
    void addNetChannel( SvNetChannel *channel );
  };

#endif // SVNETCONNECTOR_H
