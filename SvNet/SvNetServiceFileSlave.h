/*
  Project "Net packet interchange over local net and internet bridge server"
  Автор
    Sibilev A.S.

    www.salilab.com
    www.salilab.ru
  Description
    The service interacts with the channel to perform a file operations.
    This service is master service, i.e. it initiates interhange but SvNetServiceFileSlave handle this queries.
*/
#ifndef SVNETSERVICEFILESLAVE_H
#define SVNETSERVICEFILESLAVE_H

#include "SvNetService.h"


#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QDateTime>





class SvNetServiceFileSlave : public SvNetService
  {
    Q_OBJECT


  public:
    SvNetServiceFileSlave();

  public slots:
    virtual void onReceivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block );

    static SvNetDirInfo getDirInfo( const QString path );
  private:
    void sendAnswer(SvNetChannel *ch, quint32 errCode, const QString msg );

    void fileRequest( SvNetChannel *ch, const SvNetFile &src );

    void file( SvNetChannel *ch, const SvNetFile &src );

    void dirRequest( SvNetChannel *ch, const SvNetDirInfo &src );

    void makeDir( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileRemove( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileCopy( SvNetChannel *ch, const SvNetFileOper &oper );

    void fileMove( SvNetChannel *ch, const SvNetFileOper &oper );
  };

#endif // SVNETSERVICEFILESLAVE_H
