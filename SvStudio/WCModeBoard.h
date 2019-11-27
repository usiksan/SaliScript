/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef CMODEBOARD_H
#define CMODEBOARD_H

#include <QWidget>


class WCModeBoard : public QWidget
  {
    Q_OBJECT
  public:
    explicit WCModeBoard(QWidget *parent = nullptr);
    ~WCModeBoard();

    void activateNode( int node );

  signals:

  public slots:
    void onNodeListChanged();
  };

#endif // CMODEBOARD_H
