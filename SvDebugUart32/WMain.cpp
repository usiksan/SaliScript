﻿/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Главное окно программы
*/
#include "SvConfig.h"
#include "WMain.h"
#include "WCommand.h"
#include "SvHost/SvDir.h"
#include "SvUtils.h"
#include "WDPortSettings.h"
#include "SvHost/SvTextStreamIn.h"
#include "SvHost/SvTextStreamOut.h"
#include "SvDebugUart32.h"

#include <QToolBar>
#include <QActionGroup>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QFileDialog>
#include <QSettings>
#include <QToolButton>
#include <QDesktopServices>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QProcess>
#include <QProgressDialog>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>


WMain::WMain(QWidget *parent)
  : QMainWindow(parent),
    mSerial(nullptr),
    mPause(true)
  {
  //Создать систему команд
  CommandCreateActions( this, menuBar() );

  mCentral = new QSplitter();
  mDebugVar = new WDebugTable();
  //============================================
  //Колонка 1 - осциллограф
  mOscillograph = new WOscillograph( mDebugVar );
  mCentral->addWidget( mOscillograph );


  //============================================
  //Колонка 2 таблица доступных символов
  mSymbolTable = new QListWidget();
  //Будем сортировать таблицу символов, а то там хрен-чего найдешь
  mSymbolTable->setSortingEnabled(true);
  mCentral->addWidget( mSymbolTable );

  //============================================
  //Колонка 3 инструменты и переменные отладки
  mDebugSplitter = new QSplitter( Qt::Vertical, this );

  //-------------------------------------------------------
  //Список логов
  mLogList = new QListWidget();
  mDebugSplitter->addWidget( mLogList );



  //Список переменных
  mDebugSplitter->addWidget(mDebugVar);

  mCentral->addWidget(mDebugSplitter);

  //Создать центральный виджет
  setCentralWidget( mCentral );

  restorePositions();

  setupTitle();

  //Связать выбор из таблицы символов
  connect( mSymbolTable, &QListWidget::itemDoubleClicked, this, [this] (QListWidgetItem *item) {
    mDebugVar->debugAppend( item->text() );
    });

  updateRecentProject();

  //Таймер подключения
  connect( &mSerialTimer, &QTimer::timeout, this, &WMain::serialTimer );
  mSerialTimer.start(1000);

  connect( mDebugVar, &WDebugTable::queryVariable, this, &WMain::queryVariable );
  connect( mDebugVar, &WDebugTable::setVariableValue, this, &WMain::setVariable );
  }





WMain::~WMain()
  {

  }

void WMain::restorePositions()
  {
  QSettings s;
  mCentral->restoreState( s.value(CFG_TEXT_SPLITTER).toByteArray() );
  mDebugSplitter->restoreState( s.value(CFG_DEBUG_SPLITTER).toByteArray() );

  }









void WMain::fileOpenProject()
  {
  QString fname = QFileDialog::getOpenFileName( this, tr("Select project to open"), QString(),  tr("Project (*.elf)") );

  if( !fname.isEmpty() ) {
    //Похоже на проект
    fileProjectOpen( fname );
    }
  }






void WMain::fileSaveProject()
  {
  if( !mElfPath.isEmpty() ) {
    //Записать проект
    QFile file( mPrjPath );
    if( file.open( QIODevice::WriteOnly ) ) {
      file.write( QJsonDocument( mDebugVar->storeToProject() ).toJson() );
      }
    }
  }









void WMain::fileRecentProject()
  {
  //Загрузить прежний проект
  QAction *action = qobject_cast<QAction *>(sender());
  if(action)
    fileProjectOpen( action->data().toString() );
  }






void WMain::fileProjectOpen(const QString fname)
  {
  //Сохранить прежний проект
  fileSaveProject();

  QFileInfo info(fname);
  SvDir dir(info.path());

  //Загрузить в рабочий проект
  mElfPath = fname;
  mPrjPath = dir.slashedPath() + info.completeBaseName() + EXTENSION_PROJECT;
  mSymPath = dir.slashedPath() + info.completeBaseName() + QString(".symbol");
  //Исходное время модификации
  mElfTime = info.lastModified();

  QFile file(mPrjPath);
  if( file.open( QIODevice::ReadOnly ) ) {
    //Файл с перечнем отлаживаемых символов имеется, прочитать
    //Заполнить переменные в соответствии с проектом
    mDebugVar->loadFromProject( QJsonDocument::fromJson( file.readAll() ).array() );
    }

  //Обновить таблицу символов
  updateSymbolTable();

  //Установить новый заголовок
  setupTitle();

  //Добавить проект в список прежних проектов
  QSettings s;
  QStringList list;
  if( s.contains(CFG_PROJECTS) ) {
    //Получить список
    list = s.value(CFG_PROJECTS).toStringList();
    //Проверить, есть ли в списке нужный проект
    int i = findInList( fname, list );
    if( i >= 0 ) {
      //Вынести наверх
      list.removeAt( i );
      }
    else {
      //Удалить последний
      while( list.count() >= MAX_RECENT_PROJECT )
        list.removeLast();
      }
    list.insert( 0, fname );
    }
  else list.append( fname );

  s.setValue( CFG_PROJECTS, list );

  updateRecentProject();

  }




//Очистить накопления графиков
void WMain::chartsClear()
  {
  }







void WMain::debugClearLog()
  {
  mLogList->clear();
  }




void WMain::debugPause()
  {
  if( mSerial ) {
    mSerial->close();
    mSerial->deleteLater();
    mSerial = nullptr;
    }
  mPause = true;
  maDebugPause->setChecked(true);
  maDebugResume->setChecked(false);
  }




void WMain::debugResume()
  {
  mPause = false;
  maDebugPause->setChecked(false);
  maDebugResume->setChecked(true);
  }





void WMain::settings()
  {
  WDPortSettings portSettings( this );
  if( portSettings.exec() ) {
    if( mSerial ) {
      mSerial->close();
      mSerial->deleteLater();
      mSerial = nullptr;
      }
    }
  }






void WMain::helpContens()
  {
  }






void WMain::helpWeb()
  {
  //Перейти на домашнюю страницу продукта
  QDesktopServices::openUrl( QUrl(QString(DEFAULT_HOME_WEB)) );
  }






void WMain::helpAbout()
  {
  //Вывести диалог с версией
  QMessageBox::about( this, tr("About Sali SvDebugUart32"), tr("SaliLAB SvDebugUart32 is realtime embedded variable monitoring tool. /nVersion %1").arg( SV_VERSION ) );
  }






void WMain::serialTimer()
  {
  //Если порта еще нет, то подключиться
  if( mSerial == nullptr && !mPause ) {
    QSettings s;
    mSerial = new QSerialPort( s.value( CFG_DEBUG_PORT, QStringLiteral("/dev/ttyACM0") ).toString() );
    if( mSerial->open( QIODevice::ReadWrite ) ) {
      mSerial->setBaudRate( s.value( CFG_DEBUG_BAUDRATE, QStringLiteral("115200") ).toString().toInt() );
      connect( mSerial, &QSerialPort::readyRead, this, &WMain::serialRead );

      mLogList->addItem( tr("Serial port %1 baudrate %2").arg(mSerial->portName()).arg(mSerial->baudRate()) );

      //При подключении отправить запрос
      SvTextStreamOut out;
      out.begin( SV_CB_INFO_GET );
      out.end();
      mSerial->write( out.buffer() );
      }
    else {
      mSerial->deleteLater();
      mSerial = nullptr;
      }
    }

  //Проверить дату выходного файла, при необходимости обновить таблицу символов
  QFileInfo info( mElfPath );
  if( info.lastModified() > mElfTime ) {
    //Сохранить время последнего изменения
    mElfTime = info.lastModified();
    //Обновить таблицу символов
    updateSymbolTable();
    }
  }




void WMain::serialRead()
  {
  //Пока доступны строки - читаем эти строки
  while( mSerial->canReadLine() ) {

    //Прочитать строку одновременно удалив из нее спецсимволы
    QByteArray data( mSerial->readLine().simplified() );
    SvTextStreamIn in( data.data() );
    if( in.getCmd() == SV_CB_MEMORY ) {
      quint32 addr = in.getInt32();
      qint32 val = in.getInt32();
      if( mRowMap.contains(addr) )
        mDebugVar->onVariableValue( mRowMap.value(addr), addr, val );
      }
    else {
      //Это простое текстовое сообщение - добавим в лог
      mLogList->addItem( QString::fromLatin1(data) );
      //Едем в конец списка
      mLogList->scrollToItem( mLogList->item( mLogList->count() - 1) );
      if( mLogList->count() > 2000 )
        mLogList->model()->removeRow( 0 );
      }
    }
  }




void WMain::queryVariable(int row, const QString name, int arrayIndex)
  {
  if( mSymbolMap.contains(name) && mSerial ) {
    //Переменная есть в таблице
    SvSymbol sym = mSymbolMap.value(name);
    quint32 addr = sym.mAddress + arrayIndex;
    SvTextStreamOut buf;
    buf.begin( SV_CB_MEMORY_GET );
    buf.addInt32( addr );
    buf.end();
    mRowMap.insert( addr, row );
    mSerial->write( buf.buffer() );
    }
  else {
    //Переменной нету в таблице
    mDebugVar->onVariableValue( row, 0, 0 );
    }
  }




void WMain::setVariable(const QString name, int arrayIndex, int value)
  {
  if( mSymbolMap.contains(name) && mSerial ) {
    //Переменная есть в таблице
    SvSymbol sym = mSymbolMap.value(name);
    quint32 addr = sym.mAddress + arrayIndex;
    SvTextStreamOut buf;
    buf.begin( SV_CB_MEMORY );
    buf.addInt32( addr );
    buf.addInt32( value );
    buf.end();
    //qDebug() << data;
    mSerial->write( buf.buffer() );
    }
  }







void WMain::setupTitle()
  {
  QFileInfo info(mElfPath);
  setWindowTitle( QString("SvDebug " SV_VERSION "[%1]").arg( info.completeBaseName() ) );
  }




//Обновить таблицу символов
void WMain::updateSymbolTable()
  {
  QProgressDialog progress("Parsing available variables...", "Abort parsing", 0, 1000, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(10);
  progress.setValue( 1 );

  //Выполнить вывод таблицы символов
  QProcess readelf;
  readelf.start( "readelf", QStringList() << "-s" << "-W" << mElfPath );
  if( !readelf.waitForStarted() )
    return;

  if( !readelf.waitForFinished() )
    return;

  QByteArray result = readelf.readAll();

  //Убрать прежнюю таблицу символов
  mSymbolMap.clear();
  mSymbolTable->clear();

  int count = 2;
  QTextStream is( result );
  //Теперь парсим построчно данную таблицу
  while( !is.atEnd() ) {
    //   564: 20000734     4 OBJECT  GLOBAL DEFAULT   10 powerPwm
    QString line = is.readLine();
    if( line.mid( 23, 22 ) == QString("OBJECT  GLOBAL DEFAULT") ) {
      SvSymbol sm;
      sm.mAddress = line.mid( 8, 8 ).toUInt( nullptr, 16 );
      sm.mLenght  = line.mid( 17, 5 ).simplified().toInt();
      QString symbol = line.mid( 51 ).simplified();
      mSymbolMap.insert( symbol, sm );
      mSymbolTable->addItem( symbol );
      progress.setValue( count++ );
      //qDebug() << symbol << sm.mAddress << sm.mLenght;
      }
    }
  }






//Обновить список последних загруженных проектов
void WMain::updateRecentProject()
  {
  QSettings s;
  QStringList list;
  if( s.contains(CFG_PROJECTS) )
    //Получить список
    list = s.value(CFG_PROJECTS).toStringList();


  //Изменить меню прежних проектов
  for( int i = 0; i < MAX_RECENT_PROJECT; ++i )
    if( i < list.count() ) {
      QFileInfo info( list.at(i) );
      maRecentProject[i]->setText( info.completeBaseName() );
      maRecentProject[i]->setData( list.at(i) );
      maRecentProject[i]->setToolTip( list.at(i) );
      maRecentProject[i]->setVisible(true);
      }
    else maRecentProject[i]->setVisible(false);

  maFileRecentProject->setEnabled( list.count() != 0 );
  }









//Обработка требования на закрытие окна
//Реализуем сохранение файлов при выходе
void WMain::closeEvent(QCloseEvent *ev)
  {
  //Сохранить активный проект
  fileSaveProject();

  //Сохранить в настройках максимизацию окна
  QSettings s;
  s.setValue( QString(CFG_WMAIN_MAX), isMaximized() );
  s.setValue( QString(CFG_TEXT_SPLITTER), mCentral->saveState() );
  s.setValue( QString(CFG_DEBUG_SPLITTER), mDebugSplitter->saveState() );
  ev->accept();
  }




