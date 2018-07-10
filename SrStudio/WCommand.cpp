/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "WCommand.h"
#include "WMain.h"
#include "SvDebugThread.h"
#include <QMenu>
#include <QCoreApplication>
#include <QAction>
#include <QIcon>


QAction
//Переключение режимов работы
*maModeIntro,      //Режим введения
*maModeBoard,      //Режим настройки платы
*maModeText,       //Режим текстового проектирования
*maModeHelp,       //Режим помощи
*maWOscillograph,  //Окно с осциллографом


//Главное меню
*maFile,   //Меню файл
*maFileNew,
*maFileNewProject,
*maFileOpen,
*maFileOpenOver,
*maFileRecentProject,
*maFileRecentFile,
*maFileSendProject,
*maFileReceivProject,
*maFileSave,
*maFileSaveAll,
*maFileSaveAs,
*maFileSaveProject,
*maFileClose,
*maFileCloseAll,
*maFilePrint,
*maFileExit,

//Меню правка
*maEdit,
*maEditUndo,
*maEditRedo,
*maEditCut,
*maEditPaste,
*maEditCopy,
*maEditSelectAll,
*maEditSearch,
*maEditReplace,
*maEditAutoIndent,
*maEditEditorSettings,
*maEditSvQmlGenerate,

//Меню Визуальное
*maVisual,
*maVisualInsert,

//Меню сборка
*maBuild,
*maBuildBuild,  //Компиляция проекта
*maBuildAndRun,   //Компиляция и запуск
*maBuildAndPause, //Компиляция и пауза
*maBuildAndLink,  //Компиляция, линковка, если программа одинаковая, прошивка и перезапуск - если разная

//Подменю выбора канала
*maChannelSimulator, //Симулятор
*maChannelUsb,       //В аппаратуре
*maChannelRemote,    //Удаленная отладка
*maChannelInternet,  //Отладка через интернет

//Меню исполнения, отладки и симуляции
*maDebug,
*maDebugChannel,  //Канал с исполняющей системой
*maDebugLoad,     //Загрузить программу в исполняющую систему
*maDebugLink,     //Подключиться к исполняющейся программе
*maDebugFlash,    //Прошить программу в исполняющую систему

*maDebugRun,      //Компиляция и запуск исполнения
*maDebugPause,    //Остановить исполнение
*maDebugStep,     //Выполнить один шаг текущего виртуального процессора
*maDebugTrace,    //Выполнить один шаг с заходом в функцию
*maDebugReset,    //Начать исполнение заново
*maDebugAddWatch, //Добавить элемент к наблюдению
*maDebugRunAll,   //Запустить все задачи
*maDebugPauseAll, //Приостановить все задачи
*maDebugClearLog, //Очистить лог

*maErrorPrev,     //Перейти к предыдущей ошибке
*maErrorNext,     //Перрейти к следующей ошибке


//Меню Справка
*maHelp,
*maHelpContext,
*maHelpContens,
*maHelpWeb,
*maHelpAbout,

*maHelpCxtHome,     //К началу контекстной помощи
*maHelpCxtBackward, //К предыдущей странице контекстной помощи
*maHelpCxtForward,  //К следующей странице контекстной помощи

//Команды библиотекаря
*maLibHome,
*maLibBack,
*maLibForward,
*maLibApply,



*maLast;


QActionPtr maRecentProject[MAX_RECENT_PROJECT];
QActionPtr maRecentFile[MAX_RECENT_FILE];

void
CommandCreateActions(WMain *p, QMenuBar *bar) {
  QMenu *menu;
  menu = new QMenu( p->tr("File") );
  maFile = bar->addMenu( menu );

  maFileNew = menu->addAction( QIcon(QString(":/pic/new.png")), p->tr("New..."), p, SLOT(fileNew()), QString("Ctrl+N") );

  maFileNewProject = menu->addAction( QIcon(QString(":/pic/file_yellow_empty_new.png")), p->tr("New project..."), p, SLOT(fileNewProject()) );

  maFileOpen = menu->addAction( QIcon(QString(":/pic/open.png")), p->tr("Open..."), p, SLOT(fileOpen()), QString("Ctrl+O") );

  QMenu *recent = new QMenu( p->tr("Recent projects") );
  maFileRecentProject = menu->addMenu( recent );
  maFileRecentProject->setEnabled(false);
  for( int i = 0; i < MAX_RECENT_PROJECT; i++ ) {
    maRecentProject[i] = recent->addAction( QString(), p, SLOT(fileRecentProject()) );
    maRecentProject[i]->setVisible(false);
    }

  recent = new QMenu( p->tr("Recent files") );
  maFileRecentFile = menu->addMenu( recent );
  maFileRecentFile->setEnabled(false);
  for( int i = 0; i < MAX_RECENT_FILE; i++ ) {
    maRecentFile[i] = recent->addAction( QString(), p, SLOT(fileRecentFile()) );
    maRecentFile[i]->setVisible(false);
    }

  menu->addSeparator();
  maFileReceivProject = menu->addAction( QIcon(QString(":/pic/folder-import.png")), p->tr("Receiv project"), p, SLOT( fileReceivProject() ) );
  maFileSendProject   = menu->addAction( QIcon(QString(":/pic/folder-export.png")), p->tr("Send project"), p, SLOT( fileSendProject() ) );
  menu->addSeparator();
  maFileSave = menu->addAction( QIcon(QString(":/pic/save.png")), p->tr("Save"), p, SLOT(fileSave()), QString("Ctrl+S") );
  maFileSaveAs = menu->addAction( QIcon(QString(":/pic/save_as.png")), p->tr("Save as..."), p, SLOT(fileSaveAs()) );
  maFileClose = menu->addAction( QIcon(QString(":/pic/closeFile.png")), p->tr("Close"), p, SLOT(fileClose()) );
  maFileCloseAll = menu->addAction( QIcon(QString(":/pic/save_as.png")), p->tr("Close all"), p, SLOT(fileCloseAll()) );

  maFileSaveProject   = menu->addAction( QIcon(QString(":/pic/save.png")), p->tr("Save project"), p, SLOT( fileSaveProject() ) );
  menu->addSeparator();
//  *maFilePrint,
  maFileExit = menu->addAction( QIcon(QString(":/pic/exit.png")), p->tr("Exit"), p, SLOT(close()) );





  menu = new QMenu( p->tr("Edit") );
  maEdit = bar->addMenu( menu );
  maEditUndo = menu->addAction( QIcon(QString(":/pic/editUndo.png")), p->tr("Undo"), p, SLOT(editUndo()), QString("Ctrl+Z") );
  maEditRedo = menu->addAction( QIcon(QString(":/pic/editRedo.png")), p->tr("Redo"), p, SLOT(editRedo()), QString("Ctrl+X") );
  menu->addSeparator();
  maEditCopy = menu->addAction( QIcon(QString(":/pic/editCopy.png")), p->tr("Copy"), p, SLOT(editRedo()), QString("Ctrl+Ins") );
  maEditCut  = menu->addAction( QIcon(QString(":/pic/editCut.png")),  p->tr("Cut"),  p, SLOT(editCut()),  QString("Shift+Del") );
  maEditPaste = menu->addAction( QIcon(QString(":/pic/editPaste.png")),  p->tr("Paste"),  p, SLOT(editPaste()),  QString("Shift+Ins") );
  maEditSelectAll = menu->addAction( QIcon(QString(":/pic/editSelectAll.png")),  p->tr("Select All"),  p, SLOT(editSelectAll()) );
  maEditSearch = menu->addAction( QIcon(QString(":/pic/editSearch.png")),p->tr("Search"), p, SLOT(editSearchText()), QString ("Ctrl+F"));
  maEditReplace = menu->addAction( QIcon(QString(":/pic/editReplace.png")),p->tr("Replace"), p, SLOT(editReplaceText()), QString ("Ctrl+R"));
  maEditAutoIndent = menu->addAction( QIcon(QString(":/pic/editIndent.png")),p->tr("Auto indent"), p, SLOT(editAutoIndent()), QString ("Ctrl+I"));
  maEditEditorSettings = menu->addAction( p->tr("Editor settings"), p, SLOT(editAutoIndentSettings()));
  menu->addSeparator();
  maEditSvQmlGenerate  = menu->addAction( p->tr("Sv qml generate..."), p, SLOT(editSvQmlGeneration()) );



  //Для графиков
  menu = new QMenu( p->tr("Charts") );
  bar->addMenu( menu );
  menu->addAction( p->tr("Clear charts"), p, SLOT(chartsClear()) );


  menu = new QMenu( p->tr("Visual") );
  maVisual = bar->addMenu( menu );
  maVisualInsert = menu->addAction( p->tr("Insert...") );



  QMenu *chm = new QMenu( p->tr("Channel") );
  maChannelSimulator = chm->addAction( QIcon(QString(":/pic/computer_desktop.png")), p->tr("Execute on simulator"), SvDebugThread::mThread, SLOT(setDebugSimulate()) );
  maChannelUsb       = chm->addAction( QIcon(QString(":/pic/chip.png")), p->tr("Execute on hardware"), SvDebugThread::mThread, SLOT(setDebugUsb()) );
  maChannelRemote    = chm->addAction( QIcon(QString(":/pic/chip.png")), p->tr("Execute on remote"), SvDebugThread::mThread, SLOT(setDebugRemote()) );

  menu = new QMenu( p->tr("Build") );
  maBuild = bar->addMenu( menu );
  menu->addMenu( chm );
  maBuildBuild = menu->addAction( QIcon(QString(":/pic/build.png")), p->tr("Build"), p, SLOT(buildBuild()), QString("Ctrl+B") );
  maBuildAndRun = menu->addAction( QIcon(QString(":/pic/play48.png")), p->tr("Build and run"), p, SLOT(buildAndRun()), QString("F9") );
  maBuildAndPause = menu->addAction( QIcon(QString(":/pic/buildPause48.png")), p->tr("Build and pause"), p, SLOT(buildAndPause()), QString("Shift+F9") );
  maBuildAndLink  = menu->addAction( p->tr("Build and link"), p, SLOT(buildAndLink()), QString("Ctrl+F9") );

  menu = new QMenu( p->tr("Debug") );
  maDebug = bar->addMenu( menu );
  maDebugRun = menu->addAction( QIcon(QString(":/pic/taskRun.png")), p->tr("Run") );
  maDebugRun->setShortcut( QString("F5") );
  maDebugRunAll = menu->addAction( QIcon(QString(":/pic/taskRunAll.png")), p->tr("Run All") );
  maDebugPause = menu->addAction( QIcon(QString(":/pic/taskPause.png")), p->tr("Pause") );
  //maDebugPause->setShortcut();
  maDebugPauseAll = menu->addAction( QIcon(QString(":/pic/taskStop.png")), p->tr("Pause All") );
  maDebugStep = menu->addAction( QIcon(QString(":/pic/debugStep.png")), p->tr("Step") );
  maDebugStep->setShortcut( QString("F10") );
  maDebugTrace = menu->addAction( p->tr("Trace") );
  maDebugTrace->setShortcut( QString("F11") );
  menu->addSeparator();
  maDebugAddWatch = menu->addAction( QIcon(QString(":/pic/debugWatch.png")), p->tr("Add to watch") );
  maDebugAddWatch->setShortcut( QString("F3") );
  menu->addSeparator();
  maDebugClearLog = menu->addAction( QIcon(QString(":/pic/clearLog32.png")), p->tr("Clear log list") );




  menu = new QMenu( p->tr("Help") );
  maHelp = bar->addMenu( menu );

  maHelpContext = menu->addAction( QIcon(QString(":/pic/helpTab.png")), p->tr("Context help"), p, SLOT(helpContext()),  QString("F1") );
  menu->addSeparator();
  maHelpContens = menu->addAction( QIcon(QString(":/pic/help.png")), p->tr("Help contens"), p, SLOT(helpContens()) );
  menu->addSeparator();
  maHelpWeb     = menu->addAction( QIcon(QString(":/pic/website.png")), p->tr("WEB home page"), p, SLOT(helpWeb()) );
  menu->addSeparator();
  maHelpAbout   = menu->addAction( QIcon(QString(":/pic/about.png")), p->tr("About..."), p, SLOT(helpAbout()) );


  maLibHome    = new QAction( QIcon(QString(":/pic/home.png")), p->tr("To home"), 0 );
  //maLibHome->setToolTip( p->tr(""));
  maLibBack    = new QAction( QIcon(QString(":/pic/backward.png")), p->tr("To previous page"), 0 );
  maLibForward = new QAction( QIcon(QString(":/pic/forward.png")), p->tr("To forward"), 0 );
  maLibApply   = new QAction( QIcon(QString(":/pic/apply.png")), p->tr("Take object"), 0 );


  }

