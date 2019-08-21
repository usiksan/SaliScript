/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "WVisualEditor.h"
//#include "DsObject.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QFile>

WVisualEditor::WVisualEditor( QWidget *parent ) :
  QGraphicsView(parent)
  {
//  QGraphicsScene *scene = new QGraphicsScene();
//  QFile file("c:/work/SaliLab/SaliDsLibrary/DsPin.dsl");
//  if( file.open(QIODevice::ReadOnly) ) {
//    DsObject *o = new DsObject();
//    o->fromJson( QJsonDocument::fromJson( file.readAll() ).object() );
//    scene->addItem( o );
//    }
#if 0
  QGraphicsItemGroup *group = new QGraphicsItemGroup();
  group->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
  scene->addItem( group );
  QGraphicsTextItem *text = new QGraphicsTextItem( tr("Primer texta") );
  //text->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
  text->setPos( 0, -10 );
  group->addToGroup( text );

  text = new QGraphicsTextItem( tr("Another text") );
  text->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
  text->setPos( 0, 10 );
  group->addToGroup( text );
#endif
  //setScene( scene );

#if 0
  setViewport( mView = new VisualView(this) );
  mView->show();
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

  //Обновить скролл-бары
  QRect over = workProject->Sheet().OverRect();
  QSize sz = mView->size(); over.size();
  horizontalScrollBar()->setMinimum( over.left() - sz.width() );
  horizontalScrollBar()->setMaximum( over.right() + sz.width() );
  horizontalScrollBar()->setPageStep( sz.width() );

  verticalScrollBar()->setMinimum( over.top() - sz.height() );
  verticalScrollBar()->setMaximum( over.bottom() + sz.height() );
  verticalScrollBar()->setPageStep( sz.height() );

  mView->SetStart();
#endif

  }

WVisualEditor::~WVisualEditor()
  {

  }

