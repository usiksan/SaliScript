/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "WNetFileManager.h"
#include "WNetViewer.h"
#include "WNetFileList.h"
#include "DNetProcess.h"
#include "DNetProcessFileSend.h"
#include "DNetProcessFileRequest.h"
#include "DNetProcessFileDirRequest.h"
#include "DNetProcessFileOperation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>


WNetFileManager::WNetFileManager(SvMirrorManager *manger, QWidget *parent) :
  QWidget(parent),
  mEdit(false),
  mLeftFocus(false)
  {
  QVBoxLayout *vbox = new QVBoxLayout();

  //В верхней части состояние подключения
  QFormLayout *fbox = new QFormLayout();
  fbox->addRow( tr("Net status:"), mNetStatus = new QLabel( tr("Not connected") ) );

  vbox->addLayout( fbox );

  //Далее идет двухпанельный разделитель
  QSplitter *splitter = new QSplitter( Qt::Horizontal );
  vbox->addWidget( splitter, 1 );



  //Левая панель
  QWidget *panel = new QGroupBox( tr("This computer") );
  splitter->addWidget( panel );
  QVBoxLayout *vpb = new QVBoxLayout();
  fbox = new QFormLayout();
  fbox->addRow( tr("This computer id:"), mId = new QLineEdit() );
  fbox->addRow( tr("This password:"), mPassw = new QLineEdit() );
  fbox->addRow( tr("Bridge ip:"), mBridgeIp = new QLineEdit() );
  fbox->addRow( tr("Panel view:"), mLeftRemote = new QCheckBox( tr("Remote") )  );
  mLeftRemote->setChecked(false);
  connect( mLeftRemote, &QCheckBox::toggled, this, &WNetFileManager::setLeftRemote );
  vpb->addLayout( fbox );

  vpb->addWidget( mLeftDir = new QLineEdit() );

  vpb->addWidget( mLeftPanel = new WNetFileList() );
  connect( mLeftPanel, &WNetFileList::changeDirectory, this, &WNetFileManager::setLeftDir );

  panel->setLayout( vpb );



  //Правая панель
  panel = new QGroupBox( tr("Remote computer") );
  splitter->addWidget( panel );
  vpb = new QVBoxLayout();
  fbox = new QFormLayout();
  fbox->addRow( tr("Partner id:"), mLinkId = new QLineEdit() );
  fbox->addRow( tr("Partner password:"), mLinkPassw = new QLineEdit() );

  QPushButton *butLink = new QPushButton( tr("Link") );
  connect( butLink, &QPushButton::pressed, this, &WNetFileManager::cmdLink );

  QPushButton *butUnLink = new QPushButton( tr("UnLink") );
  connect( butUnLink, &QPushButton::pressed, this, &WNetFileManager::cmdUnlink );
  fbox->addRow( butLink, butUnLink );

  fbox->addRow( tr("Panel view:"), mRightRemote = new QCheckBox( tr("Remote") )  );
  mRightRemote->setChecked(false);
  connect( mRightRemote, &QCheckBox::toggled, this, &WNetFileManager::setRightRemote );
  vpb->addLayout( fbox );

  vpb->addWidget( mRightDir = new QLineEdit() );

  vpb->addWidget( mRightPanel = new WNetFileList() );
  connect( mRightPanel, &WNetFileList::changeDirectory, this, &WNetFileManager::setRightDir );
  //connect( mNetClient, &SvNetClientFile::dirList, mRightPanel, &WNetFileList::onDirList );

  panel->setLayout( vpb );


  //Внизу кнопки с функциями
  QHBoxLayout *hbox = new QHBoxLayout();
  QPushButton *but;

  but = new QPushButton( tr("View F3") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdView );
  hbox->addWidget( but );

  but = new QPushButton( tr("Edit F4") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdEdit );
  hbox->addWidget( but );

  but = new QPushButton( tr("Copy F5") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdCopy );
  hbox->addWidget( but );

  but = new QPushButton( tr("Move/Rename F6") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdMove );
  hbox->addWidget( but );

  but = new QPushButton( tr("Make directory F7") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdMkDir );
  hbox->addWidget( but );

  but = new QPushButton( tr("Delete F8") );
  connect( but, &QPushButton::pressed, this, &WNetFileManager::cmdDelete );
  hbox->addWidget( but );

  vbox->addLayout( hbox );
  setLayout( vbox );

  //TODO build

//  connect( mNetClient, &SvNetClient::linkStatus, this, &WNetFileManager::onNetStatus );
//  connect( this, &WNetFileManager::netLink, mNetClient, &SvNetClient::linkTo );
//  connect( this, &WNetFileManager::netLinkBridge, mNetClient, &SvNetClient::linkToBridge );
//  connect( this, &WNetFileManager::remoteDirGetCur, mNetClient, &SvNetClientFile::getCurrentDir );
//  connect( mNetClient, &SvNetClientFile::currentDir, this, &WNetFileManager::setRemoteDir );
//  connect( this, &WNetFileManager::remoteDirGetList, mNetClient, &SvNetClientFile::getDirList );
//  connect( mNetClient, &SvNetClientFile::dirList, this, &WNetFileManager::remoteDirList );
//  connect( this, &WNetFileManager::remoteGetFile, mNetClient, &SvNetClientFile::readFileContens );
//  connect( mNetClient, &SvNetClientFile::fileContens, this, &WNetFileManager::viewRemoteContens );
//  connect( this, &WNetFileManager::remoteDirCreate, mNetClient, &SvNetClientFile::remoteMkDir );
//  connect( this, &WNetFileManager::executeCommands, mNetClient, &SvNetClientFile::pushList );

  connect( mLeftPanel, &WNetFileList::fileView, this, &WNetFileManager::cmdView );
  connect( mRightPanel, &WNetFileList::fileView, this, &WNetFileManager::cmdView );
  connect( mLeftPanel, &WNetFileList::focused, this, [this] () { mLeftFocus = true; });
  connect( mRightPanel, &WNetFileList::focused, this, [this] () { mLeftFocus = false; });
  connect( mLeftPanel, &WNetFileList::fileEdit, this, &WNetFileManager::cmdEdit );
  connect( mRightPanel, &WNetFileList::fileEdit, this, &WNetFileManager::cmdEdit );
  connect( mLeftPanel, &WNetFileList::focusOther, mRightPanel, static_cast<void (WNetFileList::*)()>(&WNetFileList::setFocus) );
  connect( mRightPanel, &WNetFileList::focusOther, mLeftPanel, static_cast<void (WNetFileList::*)()>(&WNetFileList::setFocus) );
  connect( mLeftPanel, &WNetFileList::fileMkDir, this, &WNetFileManager::cmdMkDir );
  connect( mRightPanel, &WNetFileList::fileMkDir, this, &WNetFileManager::cmdMkDir );
  connect( mLeftPanel, &WNetFileList::fileCopy, this, &WNetFileManager::cmdCopy );
  connect( mRightPanel, &WNetFileList::fileCopy, this, &WNetFileManager::cmdCopy );
  connect( mLeftPanel, &WNetFileList::fileDelete, this, &WNetFileManager::cmdDelete );
  connect( mRightPanel, &WNetFileList::fileDelete, this, &WNetFileManager::cmdDelete );

  //Прямой ввод текущего пути
  connect( mLeftDir, &QLineEdit::editingFinished, this, [this]() {
    setLeftDir( mLeftDir->text() );
    });
  connect( mRightDir, &QLineEdit::editingFinished, this, [this]() {
    setRightDir( mRightDir->text() );
    });


  setLocalDir( QApplication::applicationDirPath() );
  }




void WNetFileManager::cmdView()
  {
  if( mLeftFocus )
    cmdViewEdit( false, mLeftRemote->isChecked(), mLeftPanel->getCurrentFile() );
  else
    cmdViewEdit( false, mRightRemote->isChecked(), mRightPanel->getCurrentFile() );
  }




void WNetFileManager::cmdEdit()
  {
  if( mLeftFocus )
    cmdViewEdit( true, mLeftRemote->isChecked(), mLeftPanel->getCurrentFile() );
  else
    cmdViewEdit( true, mRightRemote->isChecked(), mRightPanel->getCurrentFile() );
  }




void WNetFileManager::cmdViewEdit(bool edit, bool remote, const QString fname)
  {
  if( fname.isEmpty() )
    return;
  mEdit = edit;
  if( remote ) {
    DNetProcessFileRequest netProcess( fname, tr("Receiv file '%1'").arg(fname), mChannel, this );
    if( netProcess.exec() )
      viewContens( fname, netProcess.content() );
    }
  else {
    //Прочитать локальный файл
    QFile file(fname);
    if( file.open(QIODevice::ReadOnly) )
      viewContens( fname, file.readAll() );
    }
  }




void WNetFileManager::cmdCopy()
  {
  bool srcRemote;
  bool dstRemote;
  QString srcDir;
  QString srcName;
  QString dstDir;
  QString dstName;
  SvNetDirInfo list(QString{});

  if( mLeftFocus ) {
    //Копирование с левой панели на правую
    list = mLeftPanel->getSelectedFiles();
    if( list.mFileInfoList.isEmpty() ) return;
    srcRemote = mLeftRemote->isChecked();
    srcName = mLeftDir->text();
    dstName = mRightDir->text();
    dstRemote = mRightRemote->isChecked();
    }
  else {
    //Копирование с правой панели на левую
    list = mRightPanel->getSelectedFiles();
    if( list.mFileInfoList.isEmpty() ) return;
    srcRemote = mRightRemote->isChecked();
    srcName = mRightDir->text();
    dstName = mLeftDir->text();
    dstRemote = mLeftRemote->isChecked();
    }

  srcDir = srcName;
  if( !srcDir.endsWith(QChar('/')) )
    srcDir.append( QChar('/') );
  dstDir = dstName;
  if( !dstDir.endsWith(QChar('/')) )
    dstDir.append( QChar('/') );

  //TODO copy files
#if 0
  if( list.mFileInfoList.isEmpty() )
    QMessageBox::warning( this, tr("Warning"), tr("None files selected for copying.") );
  else {
    //Вывести запрос на удаление файлов
    if( list.mFileInfoList.count() > 1 ) {
      //Выделено несколько файлов, спрашиваем про количество
      if( QMessageBox::question( this, tr("Warning"), tr("Are you sure to copy %1 files into \"%2\"").arg(list.mFileInfoList.count()).arg(dstName) ) != QMessageBox::Yes )
        return;
      }
    else {
      //Выделен только один файл, спрашиваем про имя
      dstName = QInputDialog::getText( this, tr("Copy file \"%1\"").arg(list.mFileInfoList.first().mName), tr("Dest file:"), QLineEdit::Normal, dstDir + list.mFileInfoList.first().mName );
      if( dstName.isEmpty() )
        return;
      QFileInfo info(dstName);
      qint8 dir = list.first().mIsDir;
      list.first() = info;
      list.first().mIsDir = dir;
      dstName = info.absolutePath();
      dstDir = dstName;
      if( !dstDir.endsWith(QChar('/')) )
        dstDir.append( QChar('/') );
      }
    }

  if( srcRemote ) {
    if( dstRemote ) {
      //Копирование на удаленном компьютере
      SvNetOperationList oplist;
      for( const SvNetFile &file : list ) {
        if( file.mName.isEmpty() || file.mName == QString("..") )
          continue;
        SvNetClientFile::pushCopyFile( srcDir + file.fileName(), dstDir + file.fileName(), oplist );
        }
      //Добавить обновление директория назначения
      SvNetClientFile::pushDirList( dstName, oplist );
      //Список сформирован, отправить сообщение
      emit executeCommands( oplist );
      DNetProcess netProcess( mNetClient, this );
      netProcess.exec();
      }
    else {
      //Загрузка с удаленного компьютера на данный
      SvNetOperationList oplist;
      for( const SvNetFile &file : list ) {
        if( file.mName.isEmpty() || file.mName == QString("..") )
          continue;
        SvNetClientFile::pushReadFile( srcDir + file.fileName(), file.mIsDir, dstDir + file.fileName(), oplist );
        }
      //Добавить обновление директория назначения
      //updateLocalDir( dstName );
      //Список сформирован, отправить сообщение
      emit executeCommands( oplist );
      DNetProcess netProcess( mNetClient, this );
      netProcess.exec();
      }
    }
  else {
    if( dstRemote ) {
      //Загрузка на удаленный компьютер
      SvNetOperationList oplist;
      for( const SvNetFile &file : list ) {
        if( file.mName.isEmpty() || file.mName == QString("..") )
          continue;
        SvNetClientFile::pushWriteFile( srcDir + file.fileName(), dstDir + file.fileName(), oplist );
        }
      //Добавить обновление директория назначения
      SvNetClientFile::pushDirList( dstName, oplist );
      //Список сформирован, отправить сообщение
      emit executeCommands( oplist );
      DNetProcess netProcess( mNetClient, this );
      netProcess.exec();
      }
    else {
      //Вся операция проходит на локальном компьютере
      for( const SvNetFile &file : list ) {
        if( file.mName.isEmpty() || file.mName == QString("..") )
          continue;
        SvNetClientFile::localFileCopy( srcDir + file.fileName(), dstDir + file.fileName() );
        }
      }
    }

  //Обновить локальные директории
  if( !srcRemote )
    updateLocalDir( srcName );
  if( !dstRemote )
    updateLocalDir( dstName );
 #endif
  }



void WNetFileManager::cmdMove()
  {

  }



void WNetFileManager::cmdMkDir()
  {
  //Выдать диалог для ввода имени каталога
  QString name = QInputDialog::getText( this, tr("Create directory"), tr("New directory:") );
  if( !name.isEmpty() ) {
    //Директорий введен, теперь определить в левой или правой панели
    if( mLeftFocus )
      cmdMkDirInt( mLeftRemote->isChecked(), mLeftDir->text(), name );
    else
      cmdMkDirInt( mRightRemote->isChecked(), mRightDir->text(), name );
    }
  }



void WNetFileManager::cmdMkDirInt(bool remote, const QString src, const QString dst)
  {
  if( remote ) {
    //Подать команду на создание
    QString res(src);
    if( !res.endsWith(QChar('/')) && !res.endsWith(QChar(':')) )
      res.append( QChar('/') );
    res.append(dst);
    //TODO mkDirInt
//    emit remoteDirCreate( res );
//    emit remoteDirGetList( src );
//    DNetProcess netProcess( mNetClient, this );
//    netProcess.exec();
    }
  else {
    QDir dir(src);
    dir.mkdir( dst );
    updateLocalDir( src );
    }
  }



void WNetFileManager::cmdDelete()
  {
  bool remote;
  QString dirName;
  QString srcDir;
  //TODO delete files
#if 0
  SvNetFileList list;

  if( mLeftFocus ) {
    //Удаление с левой панели
    list = mLeftPanel->getSelectedFiles();
    if( list.isEmpty() ) return;
    remote = mLeftRemote->isChecked();
    srcDir = dirName = mLeftDir->text();
    if( !srcDir.endsWith(QChar('/')) )
      srcDir.append( QChar('/') );
    }
  else {
    //Удаление с правой панели
    list = mRightPanel->getSelectedFiles();
    if( list.isEmpty() ) return;
    remote = mRightRemote->isChecked();
    srcDir = dirName = mRightDir->text();
    if( !srcDir.endsWith(QChar('/')) )
      srcDir.append( QChar('/') );
    }

  if( list.isEmpty() )
    QMessageBox::warning( this, tr("Warning"), tr("None files selected for deleting.") );
  else {
    //Вывести запрос на удаление файлов
    if( list.count() > 1 ) {
      //Выделено несколько файлов, спрашиваем про количество
      if( QMessageBox::question( this, tr("Warning"), tr("Are you sure to delete %1 files").arg(list.count()) ) != QMessageBox::Yes )
        return;
      }
    else {
      //Выделен только один файл, спрашиваем про имя
      if( QMessageBox::question( this, tr("Warning"), tr("Are you sure to delete \"%1\"").arg(list.first().fileName()) ) != QMessageBox::Yes )
        return;
      }
    }

  if( remote ) {
    //Удаление на удаленном компе
    SvNetOperationList oplist;
    for( const SvNetFile &file : list ) {
      if( file.mName.isEmpty() || file.mName == QString("..") )
        continue;
      SvNetClientFile::pushDeleteFile( srcDir + file.fileName(), oplist );
      }
    //Добавить обновление текущего директория
    SvNetClientFile::pushDirList( dirName, oplist );
    //Список сформирован, отправить сообщение
    emit executeCommands( oplist );
    DNetProcess netProcess( mNetClient, this );
    netProcess.exec();
    }
  else {
    //Удаление на локальном компе
    for( const SvNetFile &file : list ) {
      if( file.mName.isEmpty() || file.mName == QString("..") )
        continue;
      if( file.mIsDir ) {
        //удалить директрий со всем содержимым
        QDir dir( srcDir + file.fileName() );
        dir.removeRecursively();
        }
      else {
        //Это простой файл
        QFile::remove( srcDir + file.fileName() );
        }
      }
    //Обновить каталог
    updateLocalDir( dirName );
    }
#endif
  }




void WNetFileManager::cmdLink()
  {
  emit netLink( mLinkId->text().toInt(), mLinkPassw->text().toInt( nullptr, 16 ) );
  if( !mRightRemote->isChecked() )
    setRightRemote( true );
  }



void WNetFileManager::cmdUnlink()
  {
  emit netLinkBridge();
  if( mLeftRemote->isChecked() )
    setLeftRemote( false );
  if( mRightRemote->isChecked() )
    setRightRemote( false );
  }




void WNetFileManager::onNetStatus(int id, int passw, QString bridge, QString link)
  {
  mId->setText( QString::number( id ) );
  mPassw->setText( QString::number( passw, 16) );
  if( bridge.isEmpty() )
    mNetStatus->setText( tr("Not connected") );
  else
    if( link.isEmpty() )
      mNetStatus->setText( tr("Bridge only \"%1\".").arg( bridge ) );
    else {
      mNetStatus->setText( tr("Bridge \"%1\". Remote \"%2\".").arg( bridge ).arg( link ) );
      emit remoteDirGetCur();
      }
  }



//Изменился директорий на левой панели
void WNetFileManager::setLeftDir(const QString dir)
  {
  mLeftDir->setText( dir );
  mLeftDir->setToolTip( dir );
  if( mLeftRemote->isChecked() ) {
    mLeftPanel->listClear();
    DNetProcessFileDirRequest netProcess( dir, tr("Receiv directory list '%1'").arg(dir), mChannel, this );
    if( netProcess.exec() )
      mLeftPanel->onDirList( dir, netProcess.dirInfo() );
    }
  else {
    //Получить директорий из локального компьютера
    mLeftPanel->onDirList( dir, SvNetHandlerFile::getDirInfo(dir) );
    mLocalDir = dir;
    }
  }



//Изменился директорий на правой панели
void WNetFileManager::setRightDir(const QString dir)
  {
  mRightDir->setText( dir );
  mRightDir->setToolTip( dir );
  if( mRightRemote->isChecked() ) {
    mRightPanel->listClear();
    DNetProcessFileDirRequest netProcess( dir, tr("Receiv directory list '%1'").arg(dir), mChannel, this );
    if( netProcess.exec() )
      mRightPanel->onDirList( dir, netProcess.dirInfo() );
    }
  else {
    //Получить директорий из локального компьютера
    mRightPanel->onDirList( dir, SvNetHandlerFile::getDirInfo(dir) );
    mLocalDir = dir;
    }
  }



void WNetFileManager::setLeftRemote(bool remote)
  {
  mLeftRemote->setChecked( remote );
  setLeftDir( remote ? mRemoteDir : mLocalDir );
  }




void WNetFileManager::setRightRemote(bool remote)
  {
  mRightRemote->setChecked( remote );
  setRightDir( remote ? mRemoteDir : mLocalDir );
  }





void WNetFileManager::viewContens(const QString fname, const QByteArray contens)
  {
  WNetViewer *edit = new WNetViewer();
  edit->setReadOnly( !mEdit );
  edit->setText( QString::fromUtf8(contens) );
  edit->setWindowTitle( fname );

  //Если разрешено редактирование, то подключить сохранение
  if( mEdit ) {
    connect( edit, &WNetViewer::saveContens, this, [] ( const QByteArray contens, const QString srcName, const QString dstName ) {
      Q_UNUSED(srcName)
      QFile file(dstName);
      if( file.open(QIODevice::WriteOnly) )
        file.write( contens );
      });
    }

  edit->show();
  edit->resize( 800, 600 );
  }





void WNetFileManager::viewRemoteContens(const QString fname, const QByteArray contens)
  {
  WNetViewer *edit = new WNetViewer();
  edit->setReadOnly( !mEdit );
  edit->setText( QString::fromUtf8(contens) );
  edit->setWindowTitle( fname );

  //Если разрешено редактирование, то подключить сохранение
  if( mEdit )
    connect( edit, &WNetViewer::saveContens, this, [this] ( const QByteArray contens, const QString srcName, const QString dstName ) {
      Q_UNUSED(srcName)
      DNetProcessFileSend netProcess( dstName, contens, tr("Send file '%1'").arg(dstName), mChannel, this );
      netProcess.exec();
      });

  edit->show();
  edit->resize( 800, 600 );
  }




//Обновить удаленный директорий
void WNetFileManager::setRemoteDir(const QString dir)
  {
  //Установить начальный удаленный каталог
  mRemoteDir = dir;
  DNetProcessFileDirRequest netProcess( dir, tr("Directory reading..."), mChannel, this );
  if( netProcess.exec() ) {
    //Установить удаленные директории в панелях
    if( mLeftRemote->isChecked() ) {
      mLeftDir->setText( dir );
      mLeftDir->setToolTip( dir );
      mLeftPanel->onDirList( dir, netProcess.dirInfo() );
      }
    if( mRightRemote->isChecked() ) {
      mRightDir->setText( dir );
      mRightDir->setToolTip( dir );
      mRightPanel->onDirList( dir, netProcess.dirInfo() );
      }
    }
  }





void WNetFileManager::setLocalDir(const QString dir)
  {
  mLocalDir = dir;
  SvNetDirInfo list = SvNetHandlerFile::getDirInfo( dir );
  //Установить локальные директории в панелях
  if( !mLeftRemote->isChecked() ) {
    mLeftDir->setText( dir );
    mLeftDir->setToolTip( dir );
    mLeftPanel->onDirList( dir, list );
    }
  if( !mRightRemote->isChecked() ) {
    mRightDir->setText( dir );
    mRightDir->setToolTip( dir );
    mRightPanel->onDirList( dir, list );
    }
  }





void WNetFileManager::updateLocalDir(const QString dir)
  {
  mLocalDir = dir;
  SvNetDirInfo list = SvNetHandlerFile::getDirInfo( dir );
  //Установить локальные директории в панелях
  if( !mLeftRemote->isChecked() && mLeftDir->text() == dir )
    mLeftPanel->onDirList( dir, list );
  if( !mRightRemote->isChecked() && mRightDir->text() == dir )
    mRightPanel->onDirList( dir, list );
  }





void WNetFileManager::updateRemoteDir(const QString dir)
  {
  DNetProcessFileDirRequest netProcess( dir, tr("Directory reading..."), mChannel, this );
  if( netProcess.exec() ) {
    if( mLeftRemote->isChecked() && mLeftDir->text() == dir )
      mLeftPanel->onDirList( dir, netProcess.dirInfo() );
    if( mRightRemote->isChecked() && mRightDir->text() == dir )
      mRightPanel->onDirList( dir, netProcess.dirInfo() );
    }
  }




