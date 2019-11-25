#ifndef SVNETLOCALMASTER_H
#define SVNETLOCALMASTER_H

#include <QObject>

class SvNetLocalMaster : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetLocalMaster(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETLOCALMASTER_H
