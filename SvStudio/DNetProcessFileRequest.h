#ifndef DNETPROCESSFILEREQUEST_H
#define DNETPROCESSFILEREQUEST_H

#include "DNetProcess.h"

class DNetProcessFileRequest : public DNetProcess
  {
    Q_OBJECT

    QString    mRemoteFileName;
    QByteArray mFileContent;
  public:
    DNetProcessFileRequest( const QString remoteFileName, const QString descr, SvNetChannel *channel, QWidget *parent );

    QByteArray content() const { return mFileContent; }
  public slots:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    virtual void receivedBlock( SvNetChannel *ch, int cmd, const QByteArray block ) override;

    //!
    //! \brief start This slot called after process dialog cretated
    //!
    virtual void start() override;

  };

#endif // DNETPROCESSFILEREQUEST_H
