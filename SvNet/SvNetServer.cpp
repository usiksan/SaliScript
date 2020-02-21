/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "SvNet управление скриптами по сети"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    SvNetServer - сервер для связи по локальной сети

    Компьютер, на котором исполняется скрипт - называется ведомым. Скрипт, на котором
    исполняется программа управления - мастер. Ведомый обеспечивает сервер, к которому
    может подключиться мастер. Данный класс представляет собой такой сервер.

    При подключении к серверу формируется канал связи с мастером, через который
    связываются обработчики.
*/
#include "SvNetServer.h"

SvNetServer::SvNetServer(int port) :
  QTcpServer()
  {

  //Настроить слушать порт
  listen( QHostAddress::Any, port );

  //Подключение на случай создания сокета
  connect( this, &QTcpServer::newConnection, this, [this] () {
    while( hasPendingConnections() ) {
      //Получить сокет
      QTcpSocket *socket = nextPendingConnection();

      //Создать канал на основе сокета
      SvNetChannel *channel = new SvNetChannel( socket );

      //Подключения для отключения канала
      connect( socket, &QTcpSocket::disconnected, channel, &SvNetChannel::deleteLater );

      //Оповестить о созданном канале
      emit addNetChannel( channel );
      }
    });
  }
