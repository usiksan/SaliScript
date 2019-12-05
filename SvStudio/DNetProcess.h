/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#ifndef DNETPROCESS_H
#define DNETPROCESS_H

#include "SvNet/SvNetServiceFileSlave.h"
#include "SvNet/SvNetChannel.h"
#include <QDialog>
#include <QProgressBar>
#include <QTextEdit>
#include <QPushButton>

class DNetProcess : public QDialog
  {
    Q_OBJECT

  protected:
    QTextEdit    *mDescr;
    QTextEdit    *mError;
    QPushButton  *mClose;
    SvNetChannel *mChannel;
  public:
    DNetProcess( const QString descr, SvNetChannel *channel, QWidget *parent );

  signals:
    /*!
       \brief sendBlock Send block with appropriate command
       \param ch        Channel which must send block or nullptr for all connected channels
       \param cmd       Command for block
       \param ar        Data block

       Block is completed part of interchange data. Each block follow with
       block command which descripting block data. Before block transfer it
       prepare with CsPacketInfo struct which contains block command and
       block size follows block data.
     */
    void sendBlock( SvNetChannel *ch, qint8 cmd, const QByteArray block );

  public slots:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    virtual void receivedBlock( SvNetChannel *ch, int cmd, const QByteArray block ) = 0;

    //!
    //! \brief start This slot called after process dialog cretated
    //!
    virtual void start() = 0;
  };

#endif // DNETPROCESS_H
