/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Обработчик обмена с экраном. Основное назначение - просмотр экрана приложения и удаленное управление приложением.
*/
#ifndef SVNETHANDLERSCREEN_H
#define SVNETHANDLERSCREEN_H

#include <QObject>

class SvNetHandlerScreen : public QObject
  {
    Q_OBJECT
  public:
    explicit SvNetHandlerScreen(QObject *parent = nullptr);

  signals:

  public slots:
  };

#endif // SVNETHANDLERSCREEN_H
