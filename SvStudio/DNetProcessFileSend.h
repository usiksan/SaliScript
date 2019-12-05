#ifndef DNETPROCESSFILESEND_H
#define DNETPROCESSFILESEND_H

#include "DNetProcess.h"

class DNetProcessFileSend : public DNetProcess
  {
    Q_OBJECT

    QString    mRemoteFileName;
    QByteArray mFileContent;
  public:
    DNetProcessFileSend( const QString remoteFileName, const QByteArray fileContent, const QString descr, SvNetChannel *channel, QWidget *parent );

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

#endif // DNETPROCESSFILESEND_H
