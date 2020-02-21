#ifndef SVNETCONNECTOR_H
#define SVNETCONNECTOR_H

#include <QObject>

class SvNetConnector : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetConnector(QObject *parent = nullptr);

  signals:

  };

#endif // SVNETCONNECTOR_H
