/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "WNetViewer.h"
#include <QKeyEvent>
#include <QMessageBox>

WNetViewer::WNetViewer() :
  QTextEdit(),
  mControl(false)
  {
  setWordWrapMode( QTextOption::NoWrap );
  }


void WNetViewer::keyPressEvent(QKeyEvent *event)
  {
  if( !isReadOnly() ) {
    if( event->key() == Qt::Key_Escape ) {
      if( document()->isModified() ) {
        if( QMessageBox::question( this, tr("Warning"), tr("File \"%1\" is modified. Save changes?").arg( windowTitle() ) ) == QMessageBox::Yes )
          emit saveContens( toPlainText().toUtf8(), windowTitle(), windowTitle() );
        }
      close();
      }
    else if( event->key() == Qt::Key_S && mControl ) {
      emit saveContens( toPlainText().toUtf8(), windowTitle(), windowTitle() );
      }
    else if( event->key() == Qt::Key_Control ) mControl = true;
    }
  else {
    if( event->key() == Qt::Key_Escape )
      close();
    }
  QTextEdit::keyPressEvent( event );
  }


void WNetViewer::keyReleaseEvent(QKeyEvent *event)
  {
  if( event->key() == Qt::Key_Control ) mControl = false;
  QTextEdit::keyReleaseEvent( event );
  }

