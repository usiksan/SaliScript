#ifndef SVNETBRIDGEMASTER_H
#define SVNETBRIDGEMASTER_H

#include <QObject>

class SvNetBridgeMaster : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetBridgeMaster(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETBRIDGEMASTER_H
