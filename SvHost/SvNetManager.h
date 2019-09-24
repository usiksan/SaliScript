/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Механизм управления каналами связи и их обработчиками

    Может обращаться к мосту и формирует сервер для подключения внешних клиентов
*/
#ifndef SVNETMANAGER_H
#define SVNETMANAGER_H

#include <QObject>
#include <QThread>

class SvNetManager : public QObject
  {
    Q_OBJECT

    QThread *mThread; //! Поток, в котором работает данный менеджер

  public:
    explicit SvNetManager(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETMANAGER_H
