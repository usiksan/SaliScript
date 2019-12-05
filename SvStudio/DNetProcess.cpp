/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "DNetProcess.h"
#include <QVBoxLayout>
#include <QTimer>

DNetProcess::DNetProcess(const QString descr, SvNetChannel *channel, QWidget *parent) :
  QDialog( parent ),
  mChannel(channel)
  {
  QVBoxLayout *vb = new QVBoxLayout();
  vb->addWidget( mDescr = new QTextEdit(descr) );
  mDescr->setReadOnly(true);
  mDescr->setWordWrapMode( QTextOption::WordWrap );

  vb->addWidget( mError = new QTextEdit() );
  mClose = new QPushButton( tr("Cancel") );
  vb->addWidget( mClose );
  setLayout( vb );
  resize( 300, 200 );

  //Connect to transfer
  connect( this, &DNetProcess::sendBlock, mChannel, &SvNetChannel::sendBlock );
  connect( mChannel, &SvNetChannel::receivedBlock, this, &DNetProcess::receivedBlock );

  //Connect to close
  connect( mClose, &QPushButton::clicked, this, &DNetProcess::reject );

  //Init transfer
  QTimer::singleShot( 30, this, &DNetProcess::start );
  }




