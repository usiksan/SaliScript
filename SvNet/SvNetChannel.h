#ifndef SVNETCHANNEL_H
#define SVNETCHANNEL_H

#include "SvNetPacketInfo.h"

#include <QObject>
#include <QTcpSocket>

class SvNetChannel : public QObject
  {
    Q_OBJECT

    QTcpSocket     *mSocket;     //! Socket, witch connection made with
  protected:
    int             mReadOffset; //! Position of next received data portion
    int             mReadSize;   //! Full read size
    QByteArray      mBlock;      //! Receiving block
    SvNetPacketInfo mPacketInfo; //! Packet info of receiving block
  public:
    explicit SvNetChannel(QObject *parent = nullptr);

    //====================================================================================
    //                         Обмен по локальной сети
    //!
    //! \brief setSocket Подключить новый сокет к каналу
    //! \param socket    Подключаемый сокет, прежний сокет удаляется, если он есть
    //!
    void        setSocket( QTcpSocket *socket );

    //!
    //! \brief getSocket Получить текущий сокет
    //! \return Текущий сокет
    //!
    QTcpSocket *getSocket() { return mSocket; }

    /*!
       \brief onBlockReceived Block received
       \param cmd Command for block
       \param is Data stream for reading block data

       This function calling on completing block reaciving. In inherited classes
       in this function must be block data decoding.
     */
    virtual void onBlockReceived( int cmd, const QByteArray block );

  signals:
    void receivBlock( qint8 cmd, QByteArray block );

  public slots:
    virtual void sendBlock( qint8 cmd, QByteArray block );

    //!
    //! \brief bridgeSlave Подключиться к мосту в качестве подчиненного устройства
    //! \param slaveId
    //! \param bridgeHost
    //!
    void bridgeSlave( const QString slaveId, const QString bridgeHost );

    //!
    //! \brief bridgeMaster
    //! \param slaveId
    //! \param bridgeHost
    //!
    void bridgeMaster( const QString slaveId, const QString bridgeHost );
  };

#endif // SVNETCHANNEL_H
