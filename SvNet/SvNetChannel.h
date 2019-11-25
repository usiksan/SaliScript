/*
  Project "Net packet interchange over local net and internet bridge server"
  Автор
    Sibilev A.S.

    www.salilab.com
    www.salilab.ru
  Description
    Object for low level information transfer
*/
#ifndef SVNETCHANNEL_H
#define SVNETCHANNEL_H

#include "SvNetPacketInfo.h"

#include <QObject>
#include <QTcpSocket>

#define SV_NET_CHANNEL_ANSWER_CMD 1

/*!
   \brief The SvNetChannel class Object for low level information transfer

   Main idea - block transfer. One piece of interchange is block transfer.
   Block may be big size. So each block may be devided onto some packets
   and transfer continuously. On received side all packets accumulate together
   and transfer completed only when full block transfered.
 */
class SvNetChannel : public QObject
  {
    Q_OBJECT

    QTcpSocket     *mSocket;     //! Socket, witch connection made with
  protected:
    int             mReadOffset; //! Position of next received data portion
    int             mReadSize;   //! Full read size
    QByteArray      mBlock;      //! Receiving block
    SvNetPacketInfo mPacketInfo; //! Packet info of receiving block
    quint8          padding[3];
  public:
    explicit SvNetChannel(QTcpSocket *socket, QObject *parent = nullptr);

    //====================================================================================
    //                         Обмен по локальной сети
    //!
    //! \brief setSocket Connect new socket to channel
    //! \param socket    Connected socket, previous socked is deleted if present.
    //!                  Channel becomes owner for socket. Do'nt delete socket manualy.
    //!
    void        setSocket( QTcpSocket *socket );

    //!
    //! \brief getSocket Get current socket
    //! \return          Current socket
    //!
    QTcpSocket *getSocket() { return mSocket; }


  signals:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    void receivedBlock( SvNetChannel *ch, int cmd, const QByteArray block );


    void receivedAnswer( SvNetChannel *ch, int srcCmd, qint32 answerCode, const QString msg );



  public slots:
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


    void sendAnswer(SvNetChannel *ch, qint8 srcCmd, qint32 answerCode, const QString msg );

  private slots:
    /*!
       \brief onReceivBytes On receiv bytes

       Calling from socket when some data ready for read. With this function
       collecting all packets for receiving block.
     */
    void onReceivBytes();

  private:
    void receiveBlockPrivate();
  };

#endif // SVNETCHANNEL_H
