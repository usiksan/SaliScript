/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef CMODEBOARD_H
#define CMODEBOARD_H

#include "SvHost/SvMirrorManager.h"

#include <QWidget>


class WCModeBoard : public QWidget
  {
    Q_OBJECT
  public:
    explicit WCModeBoard( SvMirrorManager *manager, QWidget *parent = nullptr);

  };

#endif // CMODEBOARD_H
