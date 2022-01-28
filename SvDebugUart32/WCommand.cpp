/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Список команд и конструктор меню
*/
#include "WCommand.h"
#include "WMain.h"
#include <QMenu>
#include <QCoreApplication>
#include <QAction>
#include <QIcon>
#include <QToolBar>


QAction

//Главное меню
*maFile,   //Меню файл
*maFileOpenProject,
*maFileRecentProject,
*maFileSaveProject,
*maFileExit,

//Меню Отладка
*maDebug,
*maDebugClearLog, //Очистить лог
*maDebugPause,    //Приостановить сканирование и освободить порт
*maDebugResume,   //Снова подключиться к порту и возобновить сканирование

//Меню Настройки
*maSettings,

//Меню Справка
*maHelp,
*maHelpContens,
*maHelpWeb,
*maHelpAbout,



*maLast;


QActionPtr maRecentProject[MAX_RECENT_PROJECT];

void
CommandCreateActions(WMain *p, QMenuBar *bar) {
  QMenu *menu;
  menu = new QMenu( p->tr("File") );
  maFile = bar->addMenu( menu );

  maFileOpenProject = menu->addAction( QIcon(QString(":/pic/open.png")), p->tr("Open..."), p, &WMain::fileOpenProject, QString("Ctrl+O") );

  QMenu *recent = new QMenu( p->tr("Recent projects") );
  maFileRecentProject = menu->addMenu( recent );
  maFileRecentProject->setEnabled(false);
  for( int i = 0; i < MAX_RECENT_PROJECT; i++ ) {
    maRecentProject[i] = recent->addAction( QString(), p, SLOT(fileRecentProject()) );
    maRecentProject[i]->setVisible(false);
    }


  menu->addSeparator();
  maFileSaveProject   = menu->addAction( QIcon(QString(":/pic/save.png")), p->tr("Save project"), p, &WMain::fileSaveProject, QString("Ctrl+S") );

  menu->addSeparator();
  maFileExit = menu->addAction( QIcon(QString(":/pic/exit.png")), p->tr("Exit"), p, &WMain::close );





  //Для графиков
  menu = new QMenu( p->tr("Charts") );
  bar->addMenu( menu );
  menu->addAction( p->tr("Clear charts"), p, &WMain::chartsClear );


  menu = new QMenu( p->tr("Debug") );
  maDebug = bar->addMenu( menu );
  maDebugClearLog = menu->addAction( QIcon(QString(":/pic/clearLog32.png")), p->tr("Clear log list"), p, &WMain::debugClearLog );
  maDebugPause = menu->addAction( QIcon(QString(":/pic/taskPause.png")), p->tr("Pause scan"), p, &WMain::debugPause );
  maDebugPause->setCheckable( true );
  maDebugPause->setChecked(true);
  maDebugResume = menu->addAction( QIcon(QString(":/pic/taskRun.png")), p->tr("Resume scan"), p, &WMain::debugResume );
  maDebugResume->setCheckable(true);


  menu = new QMenu( p->tr("Settings") );
  bar->addMenu( menu );
  maSettings = menu->addAction( p->tr("Settings..."), p, &WMain::settings );


  menu = new QMenu( p->tr("Help") );
  maHelp = bar->addMenu( menu );

  maHelpContens = menu->addAction( QIcon(QString(":/pic/help.png")), p->tr("Help contens"), p, SLOT(helpContens()) );
  menu->addSeparator();
  maHelpWeb     = menu->addAction( QIcon(QString(":/pic/website.png")), p->tr("WEB home page"), p, SLOT(helpWeb()) );
  menu->addSeparator();
  maHelpAbout   = menu->addAction( QIcon(QString(":/pic/about.png")), p->tr("About..."), p, SLOT(helpAbout()) );

  //Панель инструментов
  QToolBar *toolBar = new QToolBar();
  toolBar->addAction( maDebugPause );
  toolBar->addAction( maDebugResume );

  p->addToolBar( toolBar );
  }

