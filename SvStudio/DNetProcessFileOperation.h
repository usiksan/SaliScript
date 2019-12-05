#ifndef DNETPROCESSFILEOPERATION_H
#define DNETPROCESSFILEOPERATION_H

#include "DNetProcess.h"

class DNetProcessFileOperation : public DNetProcess
  {
    Q_OBJECT

    QString mSrc;
    QString mDst;
    qint8   mOper;
    char    padding[7];
  public:
    DNetProcessFileOperation( qint8 oper, const QString src, const QString dst, const QString descr, SvNetChannel *channel, QWidget *parent );

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

#endif // DNETPROCESSFILEOPERATION_H
