/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "SvNet управление скриптами по сети"
  Автор
    Alexander Sibilev
  Интернет
    www.saliLab.ru
    www.saliLab.com

  Описание
    SvNetChannel - класс канала обмена
    SvNetAnswer - структура для коротких ответов

    Блок - основа обмена информацией. Блок содержит команду - некоторый код,
    который однозначно определяет формат передаваемых данных и сами данных.
    Сами данные передается как QByteArray, в который через QDataStream записываются
    и считываются данные.

    Для обеспечения согласованности записи и чтения данных в/из блока каждый тип
    блока представляет собой структуру с согласованными функциями чтения и записи.
    Размещение функций чтения и записи в одном месте обеспечивает наглядность и резко
    снижает вероятность несогласованности операций чтения и записи.

    Структура коротких ответов - часто используемый блок данных для коротких запросов и ответов.

    Основная идея - обмен блоками. Минимальный объем передаваемой информации - это
    блок переменной длины. Поскольку блок может быть достаточно большого размера,
    то при передаче он автоматически разбивается на сетевые пакеты. Поэтому
    на приемном канале информация появляется постепенно. В задачу класса канала
    входит аккумулирование поступающих данных до тех пор, пока не будет принят весь
    блок. После принятия блока целиком он сигналом отправляется на обработку.
*/
#include "SvNetChannel.h"

#include <QDataStream>


SvNetChannel::SvNetChannel( QTcpSocket *socket, QObject *parent) :
  QObject(parent),
  mSocket(nullptr),  //Сокет, через который осуществляется связь
  mReadOffset(0),    //Позиция следующей порции приемных данных
  mReadSize(0)       //Полный размер ожидаемых данных
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






void SvNetChannel::sendBlock(SvNetChannel *ch, qint8 cmd, const QByteArray block)
  {
  if( (ch == nullptr || ch == this) && mSocket && mSocket->state() == QAbstractSocket::ConnectedState ) {
    //Подготовить заголовок блока
    SvNetBlockInfo info( cmd, block.size() );
    //Отправить заголовок блока
    mSocket->write( static_cast<const char*>( static_cast<const void*>(&info)), sizeof(SvNetBlockInfo) );
    //Отправить сам блок
    if( block.size() )
      mSocket->write( block );
    }
  }




void SvNetChannel::sendAnswer(SvNetChannel *ch, qint8 cmd, qint8 srcCmd, qint32 answerCode, const QString msg)
  {
  sendBlock( ch, cmd, SvNetAnswer( srcCmd, answerCode, msg ).buildBlock() );
  }






void SvNetChannel::onReceivBytes()
  {
  //Читаем данные пока они доступны
  while( mSocket->bytesAvailable() ) {
    //Проверяем, идет ли сейчас чтение блока
    if( mReadOffset < mReadSize ) {
      //Принята часть блока
      //Читаем данные в блок размером равным оставшейся части блока и получаем фактически прочитанную длину
      int len = static_cast<int>( mSocket->read( mBlock.data() + mReadOffset, mReadSize - mReadOffset ) );
      mReadOffset += len;
      //Если была прочитана последняя часть блока, то завершаем чтение
      if( mReadOffset >= mReadSize ) {
        receiveBlockPrivate();
        continue;
        }
      }
    else {
      //Принята информационная структура блока
      if( mSocket->read( static_cast<char*>( static_cast<void*>(&mBlockInfo)), sizeof(SvNetBlockInfo) ) != sizeof(SvNetBlockInfo) ) {
        //Нарушение синхронизации, отключаемся от сокета
        mSocket->disconnectFromHost();
        return;
        }
      else {
        //Информация о блоке принята, настроим прием данных блока
        mReadSize = mBlockInfo.lenght();
        if( mReadSize ) {
          mReadOffset = 0;
          mBlock.resize( mReadSize );
          }
        else {
          //Принят блок нулевой длины
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
  emit receivedBlock( this, mBlockInfo.command(), mBlock );
  }
