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
#include <QDataStream>

#define SV_NET_CHANNEL_ANSWER_CMD 1

struct SvNetAnswer {
    qint32  mSrcCmd;
    qint32  mAnswerCode;
    QString mMessage;

    SvNetAnswer( qint32 srcCmd, qint32 answerCode, QString msg ) : mSrcCmd(srcCmd), mAnswerCode(answerCode), mMessage(msg) {}

    SvNetAnswer( const QByteArray ar ) { parseBlock(ar); }

    QByteArray buildBlock() {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mSrcCmd
         << mAnswerCode
         << mMessage;
      return ar;
      }

    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mSrcCmd
         >> mAnswerCode
         >> mMessage;
      }

  };

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


    /*!
       \brief sendAnswer Helper function for send answer.
       \param ch         Channel through which transmission must be done.
       \param srcCmd     Source command for which answer generated.
       \param answerCode Answer code
       \param msg        Answer message

       This function packs params into SvNetAnswer structure and send it over
       net with SV_NET_CHANNEL_ANSWER_CMD command.
     */
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
