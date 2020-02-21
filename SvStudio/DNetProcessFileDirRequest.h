#ifndef DNETPROCESSFILEDIRREQUEST_H
#define DNETPROCESSFILEDIRREQUEST_H

#include "DNetProcess.h"
#include "SvNet/SvNetHandlerFile.h"

class DNetProcessFileDirRequest : public DNetProcess
  {
    Q_OBJECT

    SvNetDirInfo mDirInfo;
  public:
    DNetProcessFileDirRequest( const QString path, const QString descr, SvNetChannel *channel, QWidget *parent );

    SvNetDirInfo dirInfo() const { return mDirInfo; }
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

#endif // DNETPROCESSFILEDIRREQUEST_H
