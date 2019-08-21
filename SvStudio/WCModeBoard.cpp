/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "WCModeBoard.h"
#include "WNetFileManager.h"
#include "Host/SvNetClientMirror.h"

#include <QVBoxLayout>

WCModeBoard::WCModeBoard(QWidget *parent) : QWidget(parent)
  {
  QVBoxLayout *vb = new QVBoxLayout();
  vb->addWidget( new WNetFileManager( svNetClientMirror, this ) );
  setLayout( vb );
  }

WCModeBoard::~WCModeBoard()
  {

  }

void WCModeBoard::activateNode(int node)
  {
  Q_UNUSED(node)
  }

void WCModeBoard::onNodeListChanged()
  {

  }

