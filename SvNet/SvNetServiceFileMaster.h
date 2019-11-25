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
#ifndef SVNETSERVICEFILEMASTER_H
#define SVNETSERVICEFILEMASTER_H

#include "SvNetService.h"


class SvNetServiceFileMaster : public SvNetService
  {
    Q_OBJECT

  public:
    SvNetServiceFileMaster();

  signals:
    void receivedDirList( const QString path, const QList<SvNetFileInfo> fileList );

    void receivedMakeDir( const QString path );

    void receivedSendFile( const QString path );

  public slots:
    void requestDirList( const QString path );

    void requestMakeDir( const QString path );

    void requestSendFile( const QString path, const QByteArray fileBlock );

    void requestReceivFile( const QString path );

    void requestReceivFilePart( const QString path, qint64 offset, qint32 blockSize );

  public:
    //Return current master file service. If needed it build service
    static SvNetServiceFileMaster *get();
  };

#endif // SVNETSERVICEFILEMASTER_H
