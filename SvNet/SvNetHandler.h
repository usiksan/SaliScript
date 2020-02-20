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
#ifndef SVNETHANDLER_H
#define SVNETHANDLER_H

#include "SvNetChannel.h"

#include <QObject>
#include <QDataStream>
#include <QByteArray>

/*!
   \brief The SvNetHandler class Базовый класс обработчика запросов из сети.
                                 Унифицирует добавление канала обмена по сети.

   Канал принимает пакеты из сети, формирует из них блоки и готовые блоки отправляет на
   обработку обработчикам, порожденным от данного класса. Обработав запрос, обработчик
   формирует ответ и через канал, направивший запрос отправляет ответ.
 */
class SvNetHandler : public QObject
  {
    Q_OBJECT
  public:
    //!
    //! \brief SvNetHandler Конструктор по умолчанию
    //! \param parent       Родительский объект
    //!
    explicit SvNetHandler(QObject *parent = nullptr);

  signals:
    //!
    //! \brief sendBlock Сигнал "Отправить блок". Отправляет блок block вместе с
    //!                  командой cmd по каналу ch.
    //! \param ch        Канал, для которого предназначен блок
    //! \param cmd       Команда для блока
    //! \param block     Блок данных
    //!
    //! Канал, принимая этот сигнал проверяет, соответствует ли канал данному
    //! и, если соответствует, то упаковывает и отправляет пакетом по сети.
    void sendBlock( SvNetChannel *ch, qint8 cmd, const QByteArray block );


  public slots:
    //!
    //! \brief addNetChannel Добавить канал к обработчику.
    //! \param channel       Канал, с которого могут быть получены запросы и в который нужно
    //!                      направлять ответы
    //!
    //! Один обработчик может обслуживать несколько каналов одновременно. Поскольку ответ
    //! обработчик отправляет только для канала, с которого получен запрос, то ответ уходит
    //! строго абоненту, направившему запрос.
    void addNetChannel( SvNetChannel *channel );



    //!
    //! \brief receivedBlock Получен блок данных с канала ch и командой cmd. Данная функция должна быть
    //!                      переопределена в производных классах.
    //! \param ch            Канал, с которого получен блок данных и в который необходимо отправлять ответ
    //! \param cmd           Команда, которой соответствует блок данных
    //! \param block         Блок данных
    //!
    virtual void receivedBlock( SvNetChannel *ch, qint8 cmd, QByteArray block ) = 0;
  };

#endif // SVNETHANDLER_H
