#ifndef SVNETLOCALSLAVE_H
#define SVNETLOCALSLAVE_H

#include <QObject>

class SvNetLocalSlave : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetLocalSlave(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETLOCALSLAVE_H
