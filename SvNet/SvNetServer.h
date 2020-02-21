/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "SvNet управление скриптами по сети"
  Автор
    Alexander Sibilev
  Интернет
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
#ifndef SVNETSERVER_H
#define SVNETSERVER_H

#include "SvNetChannel.h"

#include <QTcpServer>

class SvNetServer : public QTcpServer
  {
    Q_OBJECT

  public:
    SvNetServer( int port );

  signals:
    void addChannel( SvNetChannel *channel );

  };

#endif // SVNETSERVER_H
