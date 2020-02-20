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
    SvNetHandle - базовый класс обработчика запросов из сети
*/
#include "SvNetHandler.h"

SvNetHandler::SvNetHandler(QObject *parent) : QObject(parent)
  {

  }


//!
//! \brief addNetChannel Добавить канал к обработчику.
//! \param channel       Канал, с которого могут быть получены запросы и в который нужно
//!                      направлять ответы
//!
//! Один обработчик может обслуживать несколько каналов одновременно. Поскольку ответ
//! обработчик отправляет только для канала, с которого получен запрос, то ответ уходит
//! строго абоненту, направившему запрос.
void SvNetHandler::addNetChannel(SvNetChannel *channel)
  {
  //Добавить подключения к данному каналу
  connect( channel, &SvNetChannel::receivedBlock, this, &SvNetHandler::receivedBlock );
  connect( this, &SvNetHandler::sendBlock, channel, &SvNetChannel::sendBlock );
  }
