#ifndef SVNETCLIENT_H
#define SVNETCLIENT_H

#include <QObject>

class SvNetClient : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetClient(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETCLIENT_H
