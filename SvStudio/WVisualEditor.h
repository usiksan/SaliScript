/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef WVISUALEDITOR_H
#define WVISUALEDITOR_H

#include <QGraphicsView>

class WVisualEditor : public QGraphicsView
  {
    Q_OBJECT

  public:
    WVisualEditor(QWidget *parent = 0);
    ~WVisualEditor();


  };

#endif // WVISUALEDITOR_H
