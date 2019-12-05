/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "WNetFileList.h"

#include <QDebug>

WNetFileList::WNetFileList(QWidget *parent) :
  QTableWidget(parent)
  {
  setColumnCount( 5 );
  setColumnWidth( 0, 200 );
  setColumnWidth( 1, 50 );
  setColumnWidth( 2, 50 );

  QStringList headers;
  headers << tr("Name") << tr("Ext") << tr("Size") << tr("Date") << tr("Permis");
  setHorizontalHeaderLabels( headers );

  setVerticalHeader( nullptr );

  connect( this, &WNetFileList::cellDoubleClicked, this, &WNetFileList::onDirEnter );
  //connect( this, &WNetFileList::cellPressed, this, &WNetFileList::onDirEnter );
  }




QString WNetFileList::getCurrentFile()
  {
  int row = currentRow();

  if( item( row, 2 )->text() == tr("Dir") )
    return QString();

  QString name = item( row, 0 )->text();
  QString ext  = item( row, 1 )->text();

  if( ext.isEmpty() )
    return mCurrentDir.slashedPath() + name;
  return mCurrentDir.slashedPath() + name + QString(".") + ext;
  }




SvNetDirInfo WNetFileList::getSelectedFiles() const
  {
  //Собрать выделенные файлы
  SvNetFileInfo file;
  SvNetDirInfo list(mCurrentDir.slashedPath());
  QModelIndexList sel = selectedIndexes();
  for( const QModelIndex &index : sel ) {
    int row = index.row();
    file.mName = item( row, 0 )->text();
    if( !item( row, 1 )->text().isEmpty() )
      file.mName.append( QChar('.') ).append( item( row, 1 )->text() );
    file.mFlags = item( row, 2 )->text() == tr("Dir") ? SV_NET_FF_DIR : 0;
    if( file.mName != QString("..") )
      list.mFileInfoList.append( file );
    }
  return list;
  }



void WNetFileList::listClear()
  {
  clearContents();
  }




void WNetFileList::onDirList(const QString dir, const SvNetDirInfo fileList)
  {
  qDebug() << "onDirList " << dir << fileList.mFileInfoList.count();
  mCurrentDir.set( dir );
  clearContents();
  setRowCount( fileList.mFileInfoList.count() );
  int row = 0;
  //Выбрать родительский каталог
  for( const SvNetFileInfo &file : fileList.mFileInfoList ) {
    //Наверху родительский каталог, если есть
    if( file.mName == QString("..") ) {
      addFile( file, row++ );
      break;
      }
    }

  //Вывести каталоги
  for( const SvNetFileInfo &file : fileList.mFileInfoList ) {
    if( file.mName != QString("..") && (file.mFlags & SV_NET_FF_DIR) )
      addFile( file, row++ );
    }

  //Вывести файлы
  for( const SvNetFileInfo &file : fileList.mFileInfoList ) {
    if( file.mName != QString("..") && !(file.mFlags & SV_NET_FF_DIR) )
      addFile( file, row++ );
    }
  }




void WNetFileList::onDirEnter(int row, int column)
  {
  Q_UNUSED(column)
  //Исключим ошибку "пока ничего не выделено"
  if( row < 0 || row >= rowCount() )
    return;
  QString name = item( row, 0 )->text();
  QString ext  = item( row, 1 )->text();
  if( name == QString("..") ) {
    //Поднимаемся выше по директории
    mCurrentDir.up();
    qDebug() << "up" << mCurrentDir.cleanedPath();
    emit changeDirectory( mCurrentDir.cleanedPath() );
    }
  else if( item( row, 2 )->text() == tr("Dir") ) {
    //Опускаемся ниже
    if( ext.isEmpty() )
      mCurrentDir.cd( name );
    else
      mCurrentDir.cd( name + QString(".") + ext );
    qDebug() << "dn" << mCurrentDir.cleanedPath();
    emit changeDirectory( mCurrentDir.cleanedPath() );
    }
  }



void WNetFileList::addFile(const SvNetFileInfo &file, int row)
  {
  setRowHeight( row, 17 );
  QFileInfo info(file.mName);
  if( file.mName.isEmpty() )
    setItem( row, 0, new QTableWidgetItem(QString("..")) );
  else
    setItem( row, 0, new QTableWidgetItem(info.completeBaseName()) );
  setItem( row, 1, new QTableWidgetItem(info.suffix()) );
  if( file.mFlags & SV_NET_FF_DIR ) {
    setItem( row, 2, new QTableWidgetItem(tr("Dir")) );
    }
  else {
    qint64 s = file.mSize;
    if( s < 10000 )
      setItem( row, 2, new QTableWidgetItem( QString::number(s) ) );
    else if( s < 9000000 )
      setItem( row, 2, new QTableWidgetItem( QString("%1 KB").arg(s>>10)) );
    else
      setItem( row, 2, new QTableWidgetItem( QString("%1 MB").arg(s>>20)) );
    setItem( row, 3, new QTableWidgetItem( file.mTime.toString(QString("yy-MM-dd HH:mm")) ) );
    }
  }


void WNetFileList::keyPressEvent(QKeyEvent *event)
  {
  switch( event->key() ) {
    case Qt::Key_F3 :
      emit fileView();
      break;
    case Qt::Key_F4 :
      emit fileEdit();
      break;
    case Qt::Key_F5 :
      emit fileCopy();
      break;
    case Qt::Key_F6 :
      emit fileMove( getSelectedFiles() );
      break;
    case Qt::Key_F7 :
      emit fileMkDir();
      break;
    case Qt::Key_F8 :
    case Qt::Key_Delete :
      emit fileDelete( getSelectedFiles() );
      break;
    case Qt::Key_Return :
      onDirEnter( currentRow(), 0 );
      return;
    case Qt::Key_Tab :
      emit focusOther();
      return;
    case Qt::Key_Home :
      setCurrentCell( 0,0 );
      return;
    case Qt::Key_End :
      setCurrentCell( rowCount()-1, 0 );
      return;
    }
  QTableWidget::keyPressEvent( event );
  }


void WNetFileList::focusInEvent(QFocusEvent *event)
  {
  emit focused();
  QTableWidget::focusInEvent( event );
  }
