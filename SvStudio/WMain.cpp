/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "WMain.h"
#include "WCommand.h"
#include "SvProject.h"
#include "SvPeriodicParser.h"
#include "SvCompiler/SvVpuCompiler.h"
#include "SvVMachine/SvVMachine.h"
#include "DPrjProp.h"
#include "DProcess.h"
#include "DTextEditorSettings.h"
#include "SvHost/SvMirror.h"
#include "SvHost/SvMirrorManager.h"

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


WMain::WMain(QWidget *parent)
  : QMainWindow(parent)
  {
  //Создать систему команд
  CommandCreateActions( this, menuBar() );

  //Создать основные режимы
  mCModeIntro  = new WCModeIntro();
  mCModeBoard  = new WCModeBoard( svMirrorManager );
  mCModeEditor = new WCModeEditor( this );
  mCModeHelp   = new WCModeHelp();

  connect( maDebugPause, SIGNAL(triggered()), mCModeEditor, SLOT(debugPause()) );
  connect( maDebugPauseAll, SIGNAL(triggered()), mCModeEditor, SLOT(debugPauseAll()) );
  connect( maDebugRun, SIGNAL(triggered()), mCModeEditor, SLOT(debugRun()) );
  connect( maDebugRunAll, SIGNAL(triggered()), mCModeEditor, SLOT(debugRunAll()) );
  connect( maDebugStep, SIGNAL(triggered()), mCModeEditor, SLOT(debugStep()) );
  connect( maDebugTrace, SIGNAL(triggered()), mCModeEditor, SLOT(debugTrace()) );
  connect( maDebugAddWatch, SIGNAL(triggered()), mCModeEditor, SLOT(debugAddWatch()) );

  //Создать боковой бар с режимами
  QToolBar *bar = new QToolBar( tr("Modes") );
  bar->setFloatable(false);
  bar->setMovable(false);
  bar->setIconSize( QSize(48,48) );
  bar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  maModeIntro  = bar->addAction( QIcon(QString(":/pic/intro.png")), tr("Intro"), this, SLOT(modeIntro()) );
  maModeIntro->setCheckable(true);
  maModeBoard  = bar->addAction( QIcon(QString(":/pic/hardware.png")), tr("Board"), this, SLOT(modeBoard()) );
  maModeBoard->setCheckable(true);
  maModeText   = bar->addAction( QIcon(QString(":/pic/editor.png")), tr("Editor"), this, SLOT(modeEditor()) );
  maModeText->setCheckable(true);
  maWOscillograph = bar->addAction(QIcon(":pic/charts.png"), tr("Charts"), this, SLOT(modeChart()) );
  maModeHelp   = bar->addAction( QIcon(QString(":/pic/help128.png")), tr("Help"), this, SLOT(modeHelp()) );
  maModeHelp->setCheckable(true);


  QActionGroup *group = new QActionGroup(bar);
  group->addAction( maModeIntro );
  group->addAction( maModeBoard );
  group->addAction( maModeText );
  group->addAction( maWOscillograph );
  group->addAction( maModeHelp );

  addToolBar( Qt::LeftToolBarArea, bar );

  //Создать боковой бар с командами
  bar = new QToolBar( tr("Debug") );
  bar->setFloatable(false);
  bar->setMovable(false);
  bar->setIconSize( QSize(48,48) );
  bar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  maDebugChannel = bar->addAction( maChannelSimulator->icon(), tr("Simulate"), this, SLOT(debugMode()) );
  maDebugChannel->setToolTip( tr("Executor channel type selector") );
  QMenu *chm = new QMenu();
  chm->addAction( maChannelSimulator );
  chm->addAction( maChannelUsb );
  chm->addAction( maChannelRemote );
  chm->addAction( maChannelBridge );
  QToolButton *cb = dynamic_cast<QToolButton*> ( bar->widgetForAction(maDebugChannel) );
  if( cb ) {
    cb->setMenu( chm );
    cb->setPopupMode( QToolButton::MenuButtonPopup );
    }
  bar->addAction( maBuildAndRun );
  bar->addAction( maBuildAndPause );
  bar->addAction( maBuildBuild );

  addToolBar( Qt::LeftToolBarArea, bar );

  //Создать центральный виджет
  mCentral = new QStackedWidget( this );
  mCentral->addWidget( mCModeIntro );
  mCentral->addWidget( mCModeBoard );
  mCentral->addWidget( mCModeEditor );
  mCentral->addWidget( mCModeHelp );

  setCentralWidget( mCentral );

  //Подключить сигналы
  connect( mCModeIntro, SIGNAL(newProject()), this, SLOT(fileNewProject()) );
  connect( mCModeIntro, SIGNAL(openProject()), this, SLOT(fileOpenProject()) );
  connect( mCModeIntro, &WCModeIntro::projectOpen, this, &WMain::fileProjectOpen );
  connect( svMirrorManager, &SvMirrorManager::mirrorChanged, this, &WMain::onMirrorChanged );

  //Обновить список проектов
  svProject->updateRecentProjects();

  //Обновить список файлов
  mCModeEditor->updateRecentFiles();

  //Создать и запустить периодический парсер
  SvPeriodicParser *parser = new SvPeriodicParser( mCModeEditor );
  parser->moveToThread( parser );
  parser->start();
  mCModeEditor->setParser( parser );

  //При запуске активировать режим Intro
  activateModeIntro();

  restorePositions();

  setupTitle();

  onMirrorChanged( svMirrorManager->mirrorTypeId(), svMirrorManager->mirror() );
  }





WMain::~WMain()
  {

  }

void WMain::restorePositions()
  {
  QSettings s;
  mCModeEditor->restoreState( s.value(CFG_TEXT_SPLITTER).toByteArray() );
  mCModeEditor->restoreSplitterDebug( s.value(CFG_DEBUG_SPLITTER).toByteArray() );
  }





void WMain::modeIntro()
  {
  mCentral->setCurrentIndex(0);
  }





void WMain::modeBoard()
  {
  mCentral->setCurrentIndex(1);
  }








void WMain::modeEditor()
  {
  mCentral->setCurrentIndex(2);
  }




void WMain::modeChart()
  {
  modeEditor();
  mCModeEditor->oscillographActivate();
  }


void WMain::modeHelp()
  {
  mCentral->setCurrentIndex(3);
  }






void WMain::fileOpen()
  {
  QString fname = QFileDialog::getOpenFileName( this, tr("Select file to open"), svProject->mProjectPath,
                                                tr("All SaliDs files (*.cpp *.c *.h *" EXTENSION_SCRIPT " *" EXTENSION_PROJECT ");; Headers (*.h);;Sources (*" EXTENSION_SCRIPT ");;Project (*" EXTENSION_PROJECT ")") );
  if( !fname.isEmpty() ) {
    //Проверить расширение
    if( fname.toLower().endsWith(EXTENSION_PROJECT) )
      //Похоже на проект
      fileProjectOpen( fname );
    else {
      //Обычный текстовый файл - в редактор
      mCModeEditor->fileOpen( fname );
      activateModeEditor();
      }
    }
  }






void WMain::fileOpenProject()
  {
  QString fname = QFileDialog::getOpenFileName( this, tr("Select project to open"), svProject->mProjectPath,
                                                tr("Project (*" EXTENSION_PROJECT ")") );

  if( !fname.isEmpty() ) {
    //Похоже на проект
    fileProjectOpen( fname );
    }
  }






void WMain::fileSave()
  {
  if( isText() )
    mCModeEditor->fileSave();
  }






void WMain::fileSaveAs()
  {
  if( isText() )
    mCModeEditor->fileSaveAs();
  }






void WMain::fileNew()
  {
  QString title = QFileDialog::getSaveFileName(this, tr("Create new file"), svProject->mProjectPath, "CPP Files (*.cpp *.c *" EXTENSION_SCRIPT " *.h)");

  if( title.isEmpty() ) return;

  //Создать пустой файл
  QFile file( title );
  if( file.open( QIODevice::WriteOnly ) )
    file.close();

  //Открыть его для редактирования
  mCModeEditor->fileOpen( title );
  activateModeEditor();
  }






void WMain::fileNewProject()
  {
  QString title = QFileDialog::getSaveFileName(this, tr("Create new project"), svProject->mProjectPath, tr("Project (*" EXTENSION_PROJECT ")") );

  if( title.isEmpty() ) return;

  //При необходимости добавить расширение
  if( !title.endsWith( EXTENSION_PROJECT ) )
    title.append( EXTENSION_PROJECT );

  //Создать пустой проект
  svProject->createNew( title );

  //Диалог свойств проекта
  DPrjProp dprjProp( tr("Next"), this );
  dprjProp.exec();

  //Установить тип канала связи с аппаратурой
  //Вызвать слот через очередь
  emit setMirror( svProject->mDebugType );

  //Установить новый заголовок
  setupTitle();
  }






void WMain::fileRecentProject()
  {
  //Загрузить прежний проект
  QAction *action = qobject_cast<QAction *>(sender());
  if(action)
    fileProjectOpen( action->data().toString() );
  }






void WMain::fileRecentFile()
  {
  //Загрузить прежний файл
  QAction *action = qobject_cast<QAction *>(sender());
  if(action) {
    mCModeEditor->fileOpen( action->data().toString() );
    activateModeEditor();
    }
  }









void WMain::fileSaveProject()
  {
  if( !svProject->mProjectName.isEmpty() ) {
    //Сохранить в проект открытые окна
    mCModeEditor->storeToProject();

    //Записать проект
    svProject->saveProject();
    }
  }






void WMain::fileCloseAll()
  {
  if( isText() )
    mCModeEditor->fileCloseAll();
  }






void WMain::fileClose()
  {
  if( isText() )
    mCModeEditor->fileClose();
  }






void WMain::helpContens()
  {
  //Переключить в режим отображения справки
  activateModeHelp();
  //Начать отображение с содержания
  mCModeHelp->contens();
  }






void WMain::helpWeb()
  {
  //Перейти на домашнюю страницу продукта
  QDesktopServices::openUrl( QUrl(QString(DEFAULT_HOME_WEB)) );
  }






void WMain::helpAbout()
  {
  //Вывести диалог с версией
  QMessageBox::about( this, tr("About Sali SvStudio"), tr("SaliLAB SaliScript is C programming language subset IDE\n Compiler %1\nVirtual machine %2\n SvStudio version %3").arg( TVERSION ).arg( SV_VMACHINE_VERSION ).arg( SV_VERSION ) );
  }

void WMain::mirrorLocal()
  {
  emit setMirror( SMT_LOCAL );
  }

void WMain::mirrorUsb()
  {
  emit setMirror( SMT_USB );
  }

void WMain::mirrorRemote()
  {
  emit setMirror( SMT_REMOTE );
  }

void WMain::mirrorBridge()
  {
  emit setMirror( SMT_BRIDGE );
  }





//Установить тип отладки
void WMain::onMirrorChanged(int id, SvMirrorPtr mirror)
  {
  //Подключить
  if( mirror ) {

    //В соответствии с отладчиком изменить команду
    switch( id ) {
      default :
      case SMT_LOCAL :
        maDebugChannel->setIcon( maChannelSimulator->icon() );
        maDebugChannel->setText( tr("Simulator") );
        break;

      case SMT_USB :
        maDebugChannel->setIcon( maChannelUsb->icon() );
        maDebugChannel->setText( tr("USB") );
        break;

      case SMT_REMOTE :
        maDebugChannel->setIcon( maChannelRemote->icon() );
        maDebugChannel->setText( tr("Remote") );
        break;

      case SMT_BRIDGE :
        maDebugChannel->setIcon( maChannelBridge->icon() );
        maDebugChannel->setText( tr("Bridge") );
        break;
      }


    mCModeEditor->mirrorChanged( id, mirror );

    connect( this, &WMain::setProgrammFlashRun, mirror.data(), &SvMirror::setProgrammFlashRun, Qt::QueuedConnection );
    }

  }




void WMain::compile(bool link, bool flash, bool runOrPause)
  {
  //Проверить наличие открытого проекта
  if( svProject->mProjectName.isEmpty() )
    //Проект не задан, выдать сообщение
    QMessageBox::warning( this, tr("Error!"), tr("Project is't defined! Compilation impossible. First define project.") );
  else {
    //Подготовительные операции: сохранение текстовых файлов и генерация визуальных
    mCModeEditor->fileSaveAll();

    //Компилировать
    SvProgrammPtr prog = SvCompiler6::SvVpuCompiler::build( svProject->mProjectPath, svProject->mMainScript );

    mCModeEditor->setProgramm( prog );
    //Установить новый список ошибок
    mCModeEditor->setErrors( prog->mErrors );

    //Если ошибки, то отобразить
    if( prog->mErrors.count() ) {
      //Вынести текстовый редактор на передний план
      activateModeEditor();
      }
    else if( link ) {
      //Ошибок нет, передаем программу
      //Диалог операции
      DProcess dprocess( this );

      //Отсроченная отправка программы
      QTimer::singleShot( 100, this, [this, prog, flash, runOrPause] () {
        emit setProgrammFlashRun( prog, runOrPause, flash );
        });

      dprocess.exec();
      }

    }
  }







void WMain::activateModeIntro()
  {
  maModeIntro->setChecked(true);
  modeIntro();
  }

void WMain::activateModeBoard()
  {
  maModeBoard->setChecked(true);
  modeBoard();
  }

void WMain::activateModeEditor()
  {
  maModeText->setChecked(true);
  modeEditor();
  }

void WMain::activateModeHelp()
  {
  maModeHelp->setChecked(true);
  modeHelp();
  }





void WMain::setupTitle()
  {
  setWindowTitle( QString("SvStudio " SV_VERSION "[%1]").arg(svProject->mProjectName) );
  }




//Истина, если текущий режим текстовый
bool WMain::isText() const
  {
  return mCentral->currentIndex() == 2;
  }





void WMain::fileProjectOpen(const QString fname)
  {
  //Сохранить прежний проект
  fileSaveProject();

  //Закрыть прежний проект
  fileCloseAll();

  //Загрузить в рабочий проект
  svProject->openProject( fname );

  mCModeEditor->openFromProject();

  //Установить тип канала связи с аппаратурой
  emit setMirror( svProject->mDebugType );

  //Отобразить окна
  //Включить визуальный режим как сохранено в проекте
  activateModeEditor();

  //Установить новый заголовок
  setupTitle();
  }




void WMain::editUndo()
  {
  if( isText() )
    mCModeEditor->editUndo();
  }




void WMain::editRedo()
  {
  if( isText() )
    mCModeEditor->editRedo();
  }




void WMain::editCopy()
  {
  if( isText() )
    mCModeEditor->editCopy();
  }




void WMain::editCut()
  {
  if( isText() )
    mCModeEditor->editCut();
  }




void WMain::editPaste()
  {
  if( isText() )
    mCModeEditor->editPaste();
  }




void WMain::editSelectAll()
  {
  if( isText() )
    mCModeEditor->editSelectAll();
  }





void WMain::editSearchText(){
  if( isText() )
    mCModeEditor->editSearchText();
  }



void WMain::editReplaceText()
  {
  if( isText() )
    mCModeEditor->editReplaceText();
  }



void WMain::editAutoIndent()
  {
  if( isText() )
    mCModeEditor->editAutoIndent();
  }



void WMain::editAutoIndentSettings()
  {
  DTextEditorSettings dlg(mCModeEditor, this);
  auto res = dlg.exec();
  if( res == DTextEditorSettings::Accepted ) {
    QSettings settings;
    settings.setValue(CFG_AUTOINDENT_SPACE_COUNT,dlg.autoIndentSpaceCount());
    settings.setValue(CFG_AUTOCOMPLETE_PARENTHESIS, dlg.autoIndentParenthesis());
    settings.sync();
    mCModeEditor->setAutoIndentSpaceCount(dlg.autoIndentSpaceCount());
    mCModeEditor->setAutoCompleteParenthesis(dlg.autoIndentParenthesis());
    }
  }

void WMain::editSvQmlGeneration()
  {
  if( isText() )
    mCModeEditor->editSvQmlGenerate();
  }



//Очистить накопления графиков
void WMain::chartsClear()
  {
  }




void WMain::debugMode()
  {
  //Диалог свойств проекта
  DPrjProp dprjProp( tr("Apply"), this );
  if( dprjProp.exec() )
    //Установить тип канала связи с аппаратурой
    emit setMirror( svProject->mDebugType );
  }





//Скомпилить приложение, отобразить ошибки, если есть
void WMain::buildBuild()
  {
  compile( false, false, false );
  }





void WMain::buildAndRun()
  {
  compile( true, true, true );
  //Обновить связи отладчика
  mCModeEditor->connectVars( true );
  }




void WMain::buildAndPause()
  {
  compile( true, true, false );
  //Обновить связи отладчика
  mCModeEditor->connectVars( true );
  }




void WMain::buildAndLink()
  {
  compile( true, false, true );
  //Обновить связи отладчика
  mCModeEditor->connectVars( true );
  }







void WMain::helpContext()
  {
  //В зависимости от активного окна отправляем сигнал
  mCModeEditor->helpContext();
  }




//Обработка требования на закрытие окна
//Реализуем сохранение файлов при выходе
void WMain::closeEvent(QCloseEvent *ev)
  {
  //Сохранить активный проект
  fileSaveProject();

  //Закрыть прежний проект
  fileCloseAll();

  //Проверить закрылись ли окна
  if( mCModeEditor->isAllClosed() ) {
    //Сохранить в настройках максимизацию окна
    QSettings s;
    s.setValue( QString(CFG_WMAIN_MAX), isMaximized() );
    s.setValue( QString(CFG_TEXT_SPLITTER), mCModeEditor->saveState() );
    s.setValue( QString(CFG_DEBUG_SPLITTER), mCModeEditor->stateSplitterDebug() );
    ev->accept();
    }
  }




