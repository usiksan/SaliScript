/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "WCModeBoard.h"
#include "WNetFileManager.h"

#include <QVBoxLayout>

WCModeBoard::WCModeBoard(SvMirrorManager *manager, QWidget *parent) : QWidget(parent)
  {
  QVBoxLayout *vb = new QVBoxLayout();
  vb->addWidget( new WNetFileManager( manager, this ) );
  setLayout( vb );
  }

