/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Object for low level information transfer
*/
#include "SvNetChannel.h"

#include <QDataStream>


SvNetChannel::SvNetChannel( QTcpSocket *socket, QObject *parent) :
  QObject(parent),
  mSocket(nullptr),  //Socket, witch connection made with
  mReadOffset(0),   //Position of next received data portion
  mReadSize(0)      //Full read size
  {
  if( socket )
    setSocket( socket );
  }






void SvNetChannel::setSocket(QTcpSocket *socket)
  {
  //Если есть прежний сокет, то удалить его
  if( mSocket )
    mSocket->deleteLater();

  //Назначить новый сокет
  mSocket = socket;

  //Подключит, если сокет существует
  if( mSocket ) {
    connect( mSocket, &QTcpSocket::readyRead, this, &SvNetChannel::onReceivBytes );
    connect( mSocket, &QTcpSocket::connected, this, [this] () {
      //Сбросить прерванную процедуру чтения блока
      mReadSize = mReadOffset = 0;
      });
    }
  }






void SvNetChannel::sendBlock(SvNetChannel *ch, qint8 cmd, QByteArray block)
  {
  if( (ch == nullptr || ch == this) && mSocket && mSocket->state() == QAbstractSocket::ConnectedState ) {
    //Prepare block header info
    SvNetPacketInfo info( cmd, block.size() );
    //Write block header
    mSocket->write( static_cast<const char*>( static_cast<const void*>(&info)), sizeof(SvNetPacketInfo) );
    //If block is nonzero then write block contents
    if( block.size() )
      mSocket->write( block );
    }
  }






void SvNetChannel::onReceivBytes()
  {
  //There any data from net
  while( mSocket->bytesAvailable() ) {
    //Check if block is reading
    if( mReadOffset < mReadSize ) {
      //Block part received
      int len = static_cast<int>( mSocket->read( mBlock.data() + mReadOffset, mReadSize - mReadOffset ) );
      mReadOffset += len;
      //if last part of block then parse received block
      if( mReadOffset >= mReadSize ) {
        receiveBlockPrivate();
        continue;
        }
      }
    else {
      //Packet info received
      if( mSocket->read( static_cast<char*>( static_cast<void*>(&mPacketInfo)), sizeof(SvNetPacketInfo) ) != sizeof(SvNetPacketInfo) ) {
        //Syncronization error, break connection
        mSocket->disconnectFromHost();
        return;
        }
      else {
        mReadSize = mPacketInfo.lenght();
        if( mReadSize ) {
          mReadOffset = 0;
          mBlock.resize( mReadSize );
          }
        else {
          mBlock.clear();
          receiveBlockPrivate();
          continue;
          }
        }
      }
    }
  }

void SvNetChannel::receiveBlockPrivate()
  {
  if( mPacketInfo.command() )
  emit receivedBlock( this, mPacketInfo.command(), mBlock );
  }
