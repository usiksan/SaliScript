/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "WCommand.h"
#include "WCModeEditor.h"
#include "WTextEditor.h"
#include "WOscillograph.h"
#include "Highlighter.h"
#include "SvProject.h"
#include "SvUtils.h"
#include "SvPeriodicParser.h"
#include "WCModeHelp.h"
#include "WMain.h"
#include "SvHost/SvMirrorManager.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QJsonArray>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>

#define DE_TASK_TASK  0 //Номер задачи
#define DE_TASK_RUN   1 //Состояние задачи
#define DE_TASK_IP    2 //Значение IP
#define DE_TASK_SP    3 //Значение SP
#define DE_TASK_TM    4 //Значение TM
#define DE_TASK_BP    5 //Значение BP
#define DE_TASK_LAST  6 //Количество полей


WCModeEditor::WCModeEditor(WMain *parent) :
  QSplitter(parent),
  mIps(SV_MAX_TASK,0),
  mParser(nullptr),
  mVarChangeLock(false),
  mAutoCompleteParenthesis(true),
  mAutoIndentSpaceCount(2)
  {
  QWidget *we;
  QVBoxLayout *box;
  QToolBar *bar;

  //============================================
  //Колонка 1 - текстовые редакторы
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );
  //Панель инструментов редакторов
  bar = new QToolBar( we );
  bar->addAction( maFileNew );
  bar->addAction( maFileOpen );
  bar->addAction( maFileSave );
  bar->addAction( maFileClose );
  bar->addSeparator();
  bar->addAction( maEditUndo );
  bar->addAction( maEditRedo );
  bar->addSeparator();
  bar->addAction( maEditCopy );
  bar->addAction( maEditCut );
  bar->addAction( maEditPaste );

  box->addWidget( bar );
  //Табулятор с текстовыми файлами
  mEditorTab = new QTabWidget( we );
  mEditorTab->setMinimumWidth( 500 );
  mEditorTab->setTabsClosable(true);
  mEditorTab->setMovable(true);

  // закрывать вкладки когда требуется
  connect(mEditorTab, SIGNAL(tabCloseRequested(int)), this, SLOT(fileCloseIndex(int)) );

  box->addWidget( mEditorTab );

  QStatusBar *sbar = new QStatusBar( we );
  mTextStatus = new QLabel( sbar );
  sbar->addWidget( mTextStatus );
  //sbar->addAction( maFileClose );

  box->addWidget(sbar);

  we->setLayout( box );
  addWidget( we );



  //============================================
  //Колонка 2 инструменты и переменные отладки

  //Ячкйка с инструментами
  mToolTab = new QTabWidget();

  //-------------------------------------------
  //Список файлов проекта

  //Окно списка файлов проекта
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );
  //Панель инструментов для списка файлов
  bar = new QToolBar();
  //  bar->addAction( tr("Project file list") );
  box->addWidget( bar );
  mProjectList = new QListWidget();
  box->addWidget( mProjectList );
  we->setLayout( box );
  connect( mProjectList, SIGNAL(currentRowChanged(int)), this, SLOT(fileOpenByIndex(int)) );
  mToolTab->addTab( we, QIcon(QString(":/pic/fileList.png")), tr("Files") );
  mToolTab->setTabToolTip( 0, tr("Project file list") );

  //--------------------------------------------
  //Список ошибок
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );
  mErrors = new QListWidget();
  //Панель инструментов списка ошибок
  bar = new QToolBar( we );
  maErrorPrev = bar->addAction( QIcon(":/pic/errorPrev.png"), tr("Go to prev error") );
  connect( maErrorPrev, &QAction::triggered, [&]() { mErrors->setCurrentRow( mErrors->currentRow() - 1 ); } );
  maErrorNext = bar->addAction( QIcon(":/pic/errorNext.png"), tr("Go to next error") );
  connect( maErrorNext, &QAction::triggered, [&]() { mErrors->setCurrentRow( mErrors->currentRow() + 1 ); } );
  connect( mErrors, SIGNAL(currentRowChanged(int)), this, SLOT(trackToError(int)) );

  box->addWidget( bar );
  box->addWidget( mErrors );
  we->setLayout( box );
  mToolTab->addTab( we, QIcon(":/pic/errorTab.png"), tr("Errors") );
  mToolTab->setTabToolTip( 1, tr("Error list window") );


  //---------------------------------------------
  //Система помощи
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );
  mHelp = new QTextBrowser();
  mHelp->setSource( WCModeHelp::pageHead() );
  mHelp->setOpenLinks( false );

  //Панель инструментов системы помощи
  bar = new QToolBar( we );
  maHelpCxtHome = bar->addAction( QIcon(":/pic/home.png"), tr("To home context help") );
  connect( maHelpCxtHome, &QAction::triggered, [&]() { mHelp->setSource( WCModeHelp::pageLibraryHead() ); } );
  maHelpCxtBackward = bar->addAction( QIcon(":/pic/backward.png"), tr("To prev help page"), mHelp, SLOT(backward()) );
  maHelpCxtForward = bar->addAction( QIcon(":/pic/forward.png"), tr("To next help page"), mHelp, SLOT(forward()) );
  connect( mHelp, &QTextBrowser::anchorClicked, [&]( const QUrl &url ) { qDebug() << url.fileName() << url.fragment() << url.toLocalFile() ;
    mHelp->setSource( WCModeHelp::pageConvert( url.fileName(), url.fragment() ) ); } );

  box->addWidget( bar );
  box->addWidget( mHelp );
  we->setLayout( box );
  mToolTab->addTab( we, QIcon(":/pic/helpTab.png"), tr("Help") );
  mToolTab->setTabToolTip( 2, tr("Libary help window") );

  //-------------------------------------------------------
  //Список задач
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );

  //Панель инструментов списка задач
  bar = new QToolBar( we );
  bar->insertAction( nullptr, maDebugRun );
  bar->insertAction( nullptr, maDebugRunAll );
  bar->insertAction( nullptr, maDebugPause );
  bar->insertAction( nullptr, maDebugPauseAll );

  mTasks = new QTableWidget( SV_MAX_TASK, DE_TASK_LAST );
  //Установить заголовки для окна с отлаживаемыми переменными
  QStringList taskTitles;
  taskTitles << tr("Task") << tr("Run") << tr("Ip") << tr("Sp") << tr("Tm") << tr("Bp");
  mTasks->setHorizontalHeaderLabels(taskTitles);
  //Заполнить таблицу элементами
  for( int i = 0; i < SV_MAX_TASK; ++i ) {
    mTasks->setItem( i, DE_TASK_TASK, new QTableWidgetItem() );
    mTasks->setItem( i, DE_TASK_RUN, new QTableWidgetItem() );
    mTasks->setItem( i, DE_TASK_IP, new QTableWidgetItem() );
    mTasks->setItem( i, DE_TASK_SP, new QTableWidgetItem() );
    mTasks->setItem( i, DE_TASK_TM, new QTableWidgetItem() );
    mTasks->setItem( i, DE_TASK_BP, new QTableWidgetItem() );
    mTasks->setRowHeight( i, 32 );
    }
  mTasks->setColumnWidth( DE_TASK_TASK, 35 );
  mTasks->setColumnWidth( DE_TASK_RUN, 35 );
  mTasks->setColumnWidth( DE_TASK_IP, 50 );
  mTasks->setColumnWidth( DE_TASK_SP, 50 );
  mTasks->setColumnWidth( DE_TASK_TM, 50 );
  mTasks->setColumnWidth( DE_TASK_BP, 50 );

  box->addWidget( bar );
  box->addWidget( mTasks );
  we->setLayout( box );
  mToolTab->addTab( we, QIcon(":/pic/taskTab.png"), tr("Task list") );
  mToolTab->setTabToolTip( 3, tr("Task list for active node") );


  //-------------------------------------------------------
  //Список логов
  we = new QWidget();
  //Выравниватель
  box = new QVBoxLayout();
  box->setSpacing(0);
  box->setContentsMargins( 0, 0, 0, 0 );

  //Панель инструментов списка логов
  bar = new QToolBar( we );
  bar->insertAction( nullptr, maDebugClearLog );

  mLogList = new QListWidget();
  connect( maDebugClearLog, SIGNAL(triggered()), mLogList, SLOT(clear()) );

  box->addWidget( bar );
  box->addWidget( mLogList );
  we->setLayout( box );
  mToolTab->addTab( we, QIcon(":/pic/logTab32.png"), tr("Log list") );
  mToolTab->setTabToolTip( 4, tr("Log list for active node") );


  //Список переменных
  mDebugVar = new WDebugTable();
  mDebugSplitter = new QSplitter( Qt::Vertical, this );
  mDebugSplitter->addWidget( mToolTab );
  mDebugSplitter->addWidget(mDebugVar);

  addWidget(mDebugSplitter);


  mSearchPanel = new WTextSearchPanel();
  mSearchPanel->setVisible(false);
  parent->addDockWidget(Qt::BottomDockWidgetArea, mSearchPanel);
  connect (mSearchPanel, &WTextSearchPanel::wantSearch, this, &WCModeEditor::onSearchFieldWantSearch);
  connect (mSearchPanel, &WTextSearchPanel::wantHighglight, this, &WCModeEditor::onSearchFieldWantHighlight);
  connect (mSearchPanel, &WTextSearchPanel::resultSelected, this, &WCModeEditor::onSearchFieldIndexChanged);
  connect (mSearchPanel, &WTextSearchPanel::replace, this, &WCModeEditor::onSearchReplace);
  connect (mSearchPanel, &WTextSearchPanel::replaceAll, this, &WCModeEditor::onSearchReplaceAll);

  QSettings settings;
  setAutoCompleteParenthesis(settings.value(CFG_AUTOCOMPLETE_PARENTHESIS, mAutoCompleteParenthesis).toBool());
  setAutoIndentSpaceCount(settings.value(CFG_AUTOINDENT_SPACE_COUNT, mAutoIndentSpaceCount).toInt());

  //При переключении между редакторами
  connect( mEditorTab, SIGNAL(currentChanged(int)), this, SLOT(onEditorIndex(int)) );

  }





WCModeEditor::~WCModeEditor()
  {

  }





void WCModeEditor::storeToProject()
  {
  //Список для файлов
  QJsonArray list;

  //Пройти по вкладкам, собрать файлы
  for( int i = 0; i < mEditorTab->count(); i++ ) {
    TextEditor *editor = getEditor(i);
    if( editor )
      list.append( QJsonValue(editor->mFilePath) );
    }

  //Сохранить список в проекте
  svProject->mProjectSrc.insert( WPK_FILE_LIST, QJsonValue(list) );

  //Сохранить переменные отладчика в проекте
  QJsonArray vars;
  for( int i = 0; i < mDebugVar->rowCount(); i++ ) {
    QJsonObject var;
    var.insert( QStringLiteral("name"), mDebugVar->getName(i) );
    var.insert( QStringLiteral("form"), mDebugVar->getForm(i) );
    var.insert( QStringLiteral("axiz"), mDebugVar->getAxiz(i) );
    var.insert( QStringLiteral("min"), mDebugVar->getMin(i) );
    var.insert( QStringLiteral("max"), mDebugVar->getMax(i) );
    vars.append( var );
    //    vars.append( QJsonValue(mDebugVar->getName(i)) );
    //    vars.append( QJsonValue(mDebugVar->getForm(i)) );
    }

  //Сохранить список в проекте
  svProject->mProjectSrc.insert( WPK_VARIABLE_LIST, QJsonValue(vars) );
  }





void WCModeEditor::openFromProject()
  {
  //Закрыть все открытые файлы
  fileCloseAll();

  mMainProjectFile = svProject->mMainScript;
  mFilesList.clear();
  //Открываем файлы по списку из проекта
  QJsonArray filesList = svProject->mProjectSrc.value( WPK_FILE_LIST ).toArray();
  for( int i = 0; i < filesList.count(); ++i ){
    auto path = filesList.at(i).toString();

    fileOpen( path );
    }



  //Заполнить список переменных для отладки
  QJsonArray vars = svProject->mProjectSrc.value( WPK_VARIABLE_LIST ).toArray();
  if( vars.count() == mDebugVar->rowCount() * 2 ) {
    //Список переменных в прежнем формате
    for( int i = 0; i < mDebugVar->rowCount(); i++ ) {
      mDebugVar->setVariable(i, vars.at(i*2).toString(), vars.at(i*2+1).toString(), QString(), QString(), QString() );
      }
    }
  else if( vars.count() == mDebugVar->rowCount() ) {
    //Новый расширенный список переменных, здесь-же определяется и способ отображения на графике
    for( int i = 0; i < mDebugVar->rowCount(); i++ ) {
      QJsonObject var = vars.at(i).toObject();
      mDebugVar->setVariable(i, var.value(QStringLiteral("name")).toString(),
                             var.value(QStringLiteral("form")).toString(),
                             var.value(QStringLiteral("axiz")).toString(),
                             var.value(QStringLiteral("min")).toString(),
                             var.value(QStringLiteral("max")).toString() );
      }
    }
  //Инициировать подсветку синтаксиса
  mParser->needCompile();
  }





void WCModeEditor::updateCommand()
  {
  bool editorPresent = mEditorTab->count() != 0;
  maFileClose->setEnabled( editorPresent );
  maFileCloseAll->setEnabled( editorPresent );
  maFileSaveProjectAs->setEnabled( editorPresent );
  }





void WCModeEditor::updateRecentFiles()
  {
  QSettings s;
  QStringList list;
  if( s.contains(CFG_FILES) )
    //Получить список
    list = s.value(CFG_FILES).toStringList();


  //Изменить меню прежних проектов
  for( int i = 0; i < MAX_RECENT_FILE; ++i )
    if( i < list.count() ) {
      QFileInfo info( list.at(i) );
      maRecentFile[i]->setText( info.completeBaseName() );
      maRecentFile[i]->setData( list.at(i) );
      maRecentFile[i]->setToolTip( list.at(i) );
      maRecentFile[i]->setVisible(true);
      }
    else maRecentFile[i]->setVisible(false);

  maFileRecentFile->setEnabled( list.count() != 0 );

  }





void WCModeEditor::updateProjectFileList(const QStringList & list)
  {
  mFilesList = list;

  //Очистить предыдущий список
  mProjectList->clear();
  //Сформировать новый список
  for( QString str : list ) {
    QFileInfo info(str);
    QListWidgetItem *item = new QListWidgetItem( info.fileName() );
    item->setToolTip( str );
    mProjectList->addItem( item );
    }
  }




void WCModeEditor::parsingComplete()
  {
//  emit compile();
  }










//==================================================================
//            Раздел списка задач
//==================================================================




void WCModeEditor::onMemoryChanged(SvMirror *mirror)
  {
  //Update variables
  mDebugVar->updateVariables();

  //Update tasks
  SvVpuVector vpus = mirror->vpuVector();
  int taskCount = qMin( SV_MAX_TASK, vpus.count() );
  for( int taskIndex = 0; taskIndex < taskCount; taskIndex++ ) {
    //Задача присутствует, обновить информацию
    mIps[taskIndex] = vpus.at(taskIndex).mIp;
    //Заголовок
    mTasks->item( taskIndex, DE_TASK_TASK )->setText( QString::number(taskIndex) );
    if( vpus.at(taskIndex).mDebugRun ) {
      //Задача исполняется
      mTasks->item( taskIndex, DE_TASK_RUN )->setIcon( QIcon(":/pic/taskRun.png") );
      //Очистить регистры
      mTasks->item( taskIndex, DE_TASK_IP )->setText( QString() );
      mTasks->item( taskIndex, DE_TASK_SP )->setText( QString() );
      mTasks->item( taskIndex, DE_TASK_TM )->setText( QString() );
      mTasks->item( taskIndex, DE_TASK_BP )->setText( QString() );
      }
    else {
      //Задача заторможена
      if( vpus.at(taskIndex).mIp )
        mTasks->item( taskIndex, DE_TASK_RUN )->setIcon( QIcon(":/pic/taskPause.png") );
      else
        mTasks->item( taskIndex, DE_TASK_RUN )->setIcon( QIcon(":/pic/taskStop.png") );
      //Если ip изменился с предыдущего значения, то позиционировать курсор редактора к данной строке
      if( mTasks->item( taskIndex, DE_TASK_IP )->text() != QString::number( vpus.at(taskIndex).mIp ) && !mProgramm.isNull() ) {
        //Получить имя файла, где находится текущая точка исполнения
        //Offset on 1 line is because in programm numbering start with 0 but in editor - with 1
        trackToFileLine( mProgramm->getFileName( vpus.at(taskIndex).mIp ), mProgramm->getLine( vpus.at(taskIndex).mIp ) + 1 );
        }
      mTasks->item( taskIndex, DE_TASK_IP )->setText( QString::number( vpus.at(taskIndex).mIp ) );
      mTasks->item( taskIndex, DE_TASK_SP )->setText( QString::number( vpus.at(taskIndex).mSp ) );
      mTasks->item( taskIndex, DE_TASK_TM )->setText( QString::number( vpus.at(taskIndex).mTm ) );
      mTasks->item( taskIndex, DE_TASK_BP )->setText( QString::number( vpus.at(taskIndex).mBp ) );
      }
    }

  //Clear unused task cell
  while( taskCount < SV_MAX_TASK ) {
    mTasks->item( taskCount, DE_TASK_TASK )->setText( QString() );
    mTasks->item( taskCount, DE_TASK_RUN )->setIcon( QIcon() );
    mTasks->item( taskCount, DE_TASK_IP )->setText( QString() );
    mTasks->item( taskCount, DE_TASK_SP )->setText( QString() );
    mTasks->item( taskCount, DE_TASK_TM )->setText( QString() );
    mTasks->item( taskCount, DE_TASK_BP )->setText( QString() );
    taskCount++;
    }

  }










//==================================================================
//            Раздел списка ошибок
//==================================================================
void WCModeEditor::setErrors(const SvErrorList list)
  {
  //Установить список ошибок
  mErrorList = list;

  //Иконки к ошибкам
  QIcon iconError( QString(":/pic/error.png") );
  QIcon iconWarning( QString(":/pic/warning.png") );

  //Обновить список в окне ошибок
  mErrors->clear();
  for( int i = 0; i < list.count(); i++ ) {
    //Формируем строку
    QFileInfo info( list.at(i).mFile );
    bool err = list.at(i).mError.startsWith( QString("Error") );
    QListWidgetItem *item = new QListWidgetItem( err ? iconError : iconWarning, info.completeBaseName() + QString(" : ") + list.at(i).mError );
    mErrors->addItem( item );
    }

  //Вынести список ошибок на передний план
  mToolTab->setCurrentIndex(1);

  }






//Изменилась позиция ошибки в списке ошибок
void WCModeEditor::trackToError(int errorIndex)
  {
  if( errorIndex >= 0 && errorIndex < mErrorList.count() ) {
    trackToFileLine( mErrorList.at(errorIndex).mFile, mErrorList.at(errorIndex).mLine );
    }
  }






//Перейти к заданному файлу и строке
void WCModeEditor::trackToFileLine(const QString & fname, int line)
  {
  //Вынести на передний план редактор, если нету то открыть
  fileOpenInternal( fname );
  //Перенести курсор к заданной строке
  TextEditor *editor = getEditor( mEditorTab->currentIndex() );
  if( editor )
    editor->locateLine( line );
  }







//==================================================================
//            Раздел текстового редактора
//==================================================================
QString WCModeEditor::getEditorText(const QString & fname, bool &present)
  {
  present = false;
  if( fname.isEmpty() )
    return QString();

  //Пройти по всем открытым окнам и проверить наличие файла
  for( int i = 0; i < mEditorTab->count(); i++ ) {
    TextEditor *editor = getEditor(i);
    if( editor && QString::compare( editor->mFilePath, fname, Qt::CaseInsensitive) == 0 ) {
      //Вкладка с файлом найдена, вернуть ее текст
      present = true;
      return editor->toPlainText();
      }
    }

  return QString();
  }





void WCModeEditor::fileOpen(const QString & fname)
  {
  //Если файл пустой, то ничего не делать
  if( fname.isEmpty() ) return;

  //Проверить наличие файла
  if( !QFile::exists( fname ) )
    //Файла нету, ничего не делаем
    return;

  //Открыть редактор
  fileOpenInternal( fname );

  //Добавить файл в список открытых файлов
  QStringList list;
  QSettings s;
  if( s.contains( CFG_FILES ) ) {
    list = s.value( CFG_FILES ).toStringList();
    int i = findInList( fname, list );
    if( i >= 0 )
      //Вынести наверх
      list.removeAt( i );
    else {
      //Удалить последний
      while( list.count() >= MAX_RECENT_FILE )
        list.removeLast();
      }
    list.insert( 0, fname );
    }
  else list.append( fname );

  s.setValue( CFG_FILES, list );

  updateRecentFiles();
  }




void WCModeEditor::fileOpenByIndex(int index)
  {
  if( index >= 0)
    fileOpen( mProjectList->item(index)->toolTip() );
  }





void WCModeEditor::fileSave()
  {
  //Сохранить файл из активной вкладки
  fileSaveIndex( mEditorTab->currentIndex() );
  }





void WCModeEditor::fileSaveAs()
  {
  //Получить номер активной вкладки
  int index = mEditorTab->currentIndex();

  TextEditor *editor = getEditor(index);

  if( index >= 0 && editor ) {
    QString title = editor->mFilePath;
    title = QFileDialog::getSaveFileName(this, tr("Save File"), title.isEmpty() ? (svProject->mProjectPath + svProject->mProjectName + QString(EXTENSION_SCRIPT) ) : title, QString("Script Files (*%1 *.h)").arg(EXTENSION_SCRIPT));
    if( title.isEmpty() ) return;
    editor->mFilePath = title;
    mEditorTab->setTabToolTip( index, title );

    //Сохранить файл
    fileSaveIndex( index );
    }

  }





void WCModeEditor::fileSaveAll()
  {
  for( int i = 0; i < mEditorTab->count(); i++ )
    fileSaveIndex(i);
  }





void WCModeEditor::fileClose()
  {
  fileCloseIndex( mEditorTab->currentIndex() );
  }





void WCModeEditor::fileCloseAll()
  {
  for( int i = mEditorTab->count() - 1; i >= 0; i-- )
    fileCloseIndex( i );
  }




void WCModeEditor::editUndo()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor ) {
    editor->undo();
    onEditorIndex( mEditorTab->currentIndex() );
    }
  }




void WCModeEditor::editRedo()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor ) {
    editor->redo();
    //Обновить разрешение команд undo и redo
    maEditUndo->setEnabled( editor->document()->availableUndoSteps() );
    maEditRedo->setEnabled( editor->document()->availableRedoSteps() );
    }
  }




void WCModeEditor::editCopy()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor )
    editor->copy();
  }




void WCModeEditor::editCut()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor )
    editor->cut();
  }




void WCModeEditor::editPaste()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor )
    editor->paste();
  }




void WCModeEditor::editSelectAll()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor )
    editor->selectAll();
  }



void WCModeEditor::editSearchText()
  {
  mSearchPanel->showSearch();
  }



void WCModeEditor::editReplaceText()
  {
  mSearchPanel->showReplace();
  }





void WCModeEditor::editAutoIndent(){
  TextEditor *editor = getCurrentEditor();
  if( editor ) {
    editor->autoIndent(editor->textCursor().selectionStart(), editor->textCursor().selectionEnd());
    }
  }





void WCModeEditor::editSvQmlGenerate()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor != nullptr )
    editor->editSvQmlGenerate();
  }




void WCModeEditor::fileSaveIndex(int tabIndex)
  {
  if( tabIndex >= 0 ) {
    TextEditor *editor = getEditor( tabIndex );
    if( editor == nullptr ) return;
    QString title = editor->mFilePath; // mTab->tabText(index);
    //Сохранить файл
    QFile file(title);
    if( editor && file.open( QIODevice::WriteOnly | QFile::Text ) ) {
      //При записи преобразуем в utf8
      file.write( editor->toPlainText().toUtf8() );
      file.close();
      //Обновить время модификации
      QFileInfo info( file );
      editor->setFileTime( info.lastModified() );
      //Сбросить флаг изменения
      editor->document()->setModified( false );
      }
    }
  }





void WCModeEditor::fileCloseIndex(int tabIndex)
  {
  TextEditor *editor = getEditor(tabIndex);
  if( editor && editor->document()->isModified() ) {
    int r = QMessageBox::question( this, tr("Warning!"), tr("File \"%1\" is modified! Do you want to save changes?").arg(editor->mFilePath), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
    if( r == QMessageBox::Cancel ) return;
    if( r == QMessageBox::Yes )
      fileSaveIndex(tabIndex);
    }
  mEditorTab->removeTab(tabIndex);
  if( editor )
    delete editor;
  }





//При изменении вкладки с редактором
void WCModeEditor::onEditorIndex(int tabIndex)
  {
  TextEditor *editor = getEditor(tabIndex);
  if( editor ) {
    mSearchPanel->invalidateSearchResults();
    //Обновить разрешение команд undo и redo
    maEditUndo->setEnabled( editor->isUndoAvailable() );
    maEditRedo->setEnabled( editor->isRedoAvailable() );
    //Команды копирования и вырезания
    maEditCopy->setEnabled( editor->isSelectPresent() );
    maEditCut->setEnabled( editor->isSelectPresent() );
    }
  else {
    //Обновить разрешение команд undo и redo
    maEditUndo->setEnabled( false );
    maEditRedo->setEnabled( false );
    //Команды копирования и вырезания
    maEditCopy->setEnabled( false );
    maEditCut->setEnabled( false );
    }
  }




//При изменении undo, redo
void WCModeEditor::onUndoRedoChanged(bool)
  {
  TextEditor *editor = getCurrentEditor();
  if( editor ) {
    //Обновить разрешение команд undo и redo
    maEditUndo->setEnabled( editor->isUndoAvailable() );
    maEditRedo->setEnabled( editor->isRedoAvailable() );
    }
  }





//При изменении выделения
void WCModeEditor::onSelectChanged(bool)
  {
  TextEditor *editor = getCurrentEditor();
  if( editor ) {
    maEditCopy->setEnabled( editor->isSelectPresent() );
    maEditCut->setEnabled( editor->isSelectPresent() );
    }
  }




void WCModeEditor::textChanged()
  {
  mSearchPanel->invalidateSearchResults();
  }




void WCModeEditor::mirrorChanged(int id, SvMirrorPtr mirrorPtr)
  {
  Q_UNUSED(id)
//  qDebug() << "mirrorChanged" << id << mirrorPtr.data();
  //При изменении памяти
  mDebugVar->setupMirror(mirrorPtr);

  //При поступлении loga
  connect( mirrorPtr.data(), &SvMirror::log, this, &WCModeEditor::onLog );
  //При изменении текстового статуса
  connect( mirrorPtr.data(), &SvMirror::linkChanged, this, &WCModeEditor::onTextStatusChanged );

  connect( mirrorPtr.data(), &SvMirror::memoryChanged, this, &WCModeEditor::onMemoryChanged );

  connect( this, &WCModeEditor::debug, mirrorPtr.data(), &SvMirror::debug );

  //Обновить текстовый статус
  onTextStatusChanged( false, QString{}, QString{} );
  }









//Получить текстовый редактор с заданным индексом
TextEditor *WCModeEditor::getEditor(int tabIndex)
  {
  return dynamic_cast<TextEditor*>( mEditorTab->widget(tabIndex) );
  }




//Получить текущий текстовый редактор
TextEditor *WCModeEditor::getCurrentEditor()
  {
  return dynamic_cast<TextEditor*>( mEditorTab->widget( mEditorTab->currentIndex() ) );
  }





//Получить идентификатор под курсором для текущего текстового редактора
QString WCModeEditor::getWordOverCursor()
  {
  TextEditor *editor = getCurrentEditor();
  if( editor )
    return editor->getWordCursorOver();
  return QString();
  }




void WCModeEditor::fileOpenInternal(const QString & fname)
  {
  qDebug() << "open" <<fname;
  if( fname.isEmpty() ) return;
  //Пройти по всем открытым окнам и проверить наличие файла
  for( int i = 0; i < mEditorTab->count(); i++ ) {
    TextEditor *editor = getEditor(i);
    if( editor && QString::compare( editor->mFilePath, fname, Qt::CaseInsensitive) == 0 ) {
      //Вкладка с файлом найдена, вынести на передний план
      mEditorTab->setCurrentIndex( i );
      return;
      }
    }

  //Создать новый редактор
  QFile file(fname);
  if( file.open(QFile::ReadOnly | QFile::Text) ) {
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    TextEditor *editor = new TextEditor();
    editor->setAutoCompleteParenthesis(mAutoCompleteParenthesis);
    editor->setAutoIndentSpaceCount(mAutoIndentSpaceCount);
    editor->setFont( font );
    editor->mFilePath = fname;
    QFileInfo info(file);
    editor->setFileTime( info.lastModified() );

    Highlighter *hl = new Highlighter( editor->document() );
    //connect( this, SIGNAL(compile()), hl, SLOT(rehighlight()) );
    connect( editor, SIGNAL(rehighlightBlock(QTextBlock)), hl, SLOT(rehighlightBlock(QTextBlock)) );
    connect( editor, &TextEditor::jump, this, &WCModeEditor::trackToFileLine );
    connect( editor, &TextEditor::setLink, hl, &Highlighter::setLink );

    QByteArray allFile = file.readAll();
    //Проверить кодировку utf-8
    bool utf8 = false;
    for( int i = 0; i < allFile.count(); ++i ) {
      int ch = allFile.at(i) & 0xff;
      if( ch >= 0x80 && ch < 0xc0 ) {
        utf8 = true;
        break;
        }
      }

    //Загрузить в соответствующей кодировке
    if( utf8 )
      editor->setPlainText( QString::fromUtf8(allFile) );
    else
      editor->setPlainText( QString::fromLocal8Bit(allFile) );

    if(mParser ) {
      connect( mParser, SIGNAL(compileComplete()), hl, SLOT(rehighlight()) );
      connect( editor, SIGNAL(contensChanged()), mParser, SLOT(needCompile()) );
      connect( editor, &TextEditor::fileOpen, this, &WCModeEditor::fileOpen );
      connect( editor->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()) );
      }

    //При изменении состояния редакторов
    connect( editor, SIGNAL(redoAvailable(bool)), this, SLOT(onUndoRedoChanged(bool)) );
    connect( editor, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoRedoChanged(bool)) );
    connect( editor, SIGNAL(copyAvailable(bool)), this, SLOT(onSelectChanged(bool)) );

    int i = mEditorTab->addTab( editor, info.fileName() );
    mEditorTab->setTabToolTip( i, info.absoluteFilePath() );
    mEditorTab->setCurrentIndex( i );
    }
  }





//==================================================================
//            Раздел отладчика





void WCModeEditor::connectVars(bool link)
  {
  if( link ) {
    //Пройти по списку переменных, получить адреса, получить значения
    //регистрировать для мониторинга

    mDebugVar->debugVariables();
    }
  else {
    mDebugVar->clearVariables();
    }

  }




void WCModeEditor::onLog( const QString msg )
  {
  mLogList->addItem( msg );
  }



//Показать помощь по символу под курсором
void WCModeEditor::helpContext()
  {
  QString ident = getWordOverCursor();
  qDebug() << "Context" << ident;
  }




//Добавить в отладку символ под курсором
void WCModeEditor::debugAddWatch()
  {
  QString ident = getWordOverCursor();
  mDebugVar->debugAppend(ident);
  }



//Пуск VPU
void WCModeEditor::debugRun()
  {
  emit debug( task(), SDC_RUN, 0, 0 );
  }



//Пуск всех VPU
void WCModeEditor::debugRunAll()
  {
  for( int i = 0; i < SV_MAX_TASK; i++ )
    emit debug( i, SDC_RUN, 0, 0 );
  }




//Шаг программы
void WCModeEditor::debugStep()
  {
  if( mProgramm.isNull() )
    return;
  //Задача заторможена
  int ip = mIps.at(task());
  int line = mProgramm->getLine(ip);
  int file = mProgramm->getFile(ip);
  //Пропустить текущую строку
  int ipe = ip + 1;
  while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
  emit debug( task(), SDC_RUN_STEP, ip, ipe );
  }




//Шаг программы с заходом в функцию
void WCModeEditor::debugTrace()
  {
  if( mProgramm.isNull() )
    return;
  //Задача заторможена
  int ip = mIps.at(task());
  int line = mProgramm->getLine(ip);
  int file = mProgramm->getFile(ip);
  //Пропустить текущую строку
  int ipe = ip + 1;
  while( mProgramm->getLine(ipe) == line && mProgramm->getFile(ipe) == file ) ipe++;
  emit debug( task(), SDC_RUN_TRACE, ip, ipe );
  }




//Пауза VPU
void WCModeEditor::debugPause()
  {
  if( mProgramm.isNull() )
    return;
  emit debug( task(), SDC_RUN_UNTIL, 0, mProgramm->codeCount() );
  }



//Пауза всех VPU
void WCModeEditor::debugPauseAll()
  {
  if( mProgramm.isNull() )
    return;
  for( int i = 0; i < SV_MAX_TASK; i++ )
    emit debug( i, SDC_RUN_UNTIL, 0, mProgramm->codeCount() );
  }





//При изменении текстового статуса
void WCModeEditor::onTextStatusChanged(bool linked, const QString controllerType, const QString loadedProgramm)
  {
  if( linked )
    mTextStatus->setText( tr("Linked to %1. Programm: %2").arg( controllerType ).arg( loadedProgramm ) );
  else
    mTextStatus->setText( tr("Not connected") );
  }



//Получить номер активной задачи
int WCModeEditor::task()
  {
  int row = mTasks->currentRow();
  if( row >= 0 && row < mIps.count() )
    return row;
  return 0;
  }




void WCModeEditor::onSearchFieldWantSearch(const QString &text)
  {
  auto pEditor = getCurrentEditor();
  if( pEditor == nullptr )
    return;
  auto results = searchTextCurrentEditor(text, mSearchPanel->isCaseSensitive(), mSearchPanel->isFullWord());
  mSearchPanel->setResults(results);
  }




void WCModeEditor::onSearchFieldWantHighlight(const QList<TextSearchResults> &results)
  {
  auto pEditor = getCurrentEditor();
  if (pEditor == nullptr)
    return;
  pEditor->highlightSearchResults(results);
  }




void WCModeEditor::onSearchFieldIndexChanged(int index)
  {
  auto pEditor = getCurrentEditor();
  if (pEditor == nullptr)
    return;
  auto result = mSearchPanel->result(index);
  if (result.mStartIndex >= 0){
    auto cursor = pEditor->textCursor();
    cursor.setPosition(result.mStartIndex);
    pEditor->setTextCursor(cursor);
    }
  pEditor->highlightSearchResults(mSearchPanel->getResults(), index);
  }




void WCModeEditor::onSearchReplace(const QString &text, const TextSearchResults &searchResult)
  {
  fileOpen(searchResult.mFile);
  TextEditor* pEditor = getCurrentEditor();
  if (pEditor == nullptr)
    return;
  pEditor->replace(searchResult.mStartIndex, searchResult.mEndIndex, text);
  }






void WCModeEditor::onSearchReplaceAll(const QString &text, const QList<TextSearchResults> &results)
  {
  QMap<QString, QList<TextSearchResults>> resultsByFiles;
  foreach (auto result, results) {
    if (!resultsByFiles.contains(result.mFile)){
      resultsByFiles[result.mFile] = QList<TextSearchResults>();
      }
    resultsByFiles[result.mFile].append(result);
    }
  foreach (auto file, resultsByFiles.keys()) {
    fileOpen(file);
    TextEditor* pEditor = getCurrentEditor();
    if (pEditor == nullptr)
      continue;
    pEditor->replaceAll(resultsByFiles[file], text);
    }
  }





QList<TextSearchResults> WCModeEditor::searchTextCurrentEditor(const QString &text, bool matchCase, bool fullWord)
  {

  QList<TextSearchResults> results;
  auto pEditor = getCurrentEditor();
  if (pEditor == nullptr || text.length() == 0)
    return results;
  auto editorText = pEditor->toPlainText();
  int index = -text.length();

  auto doc = pEditor->document();
  auto cursor  = pEditor->textCursor();
  auto caseSensitive = matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
  do{
    index = editorText.indexOf(text, index + text.length(), caseSensitive);
    if (index >= 0){

      cursor.setPosition(index);
      int curLine = cursor.blockNumber();
      auto word = pEditor->getWordCursorOver(cursor);
      if (!fullWord || (fullWord && word == text)){
        QTextBlock tb = doc->findBlockByLineNumber(curLine);
        QString lineText = tb.text();
        results.append(TextSearchResults(pEditor->mFilePath, index, index + text.length(), curLine + 1, lineText));
        }
      }
    }while (index >= 0);
  return results;
  }





int WCModeEditor::autoIndentSpaceCount() const
  {
  return mAutoIndentSpaceCount;
  }




void WCModeEditor::setAutoIndentSpaceCount(int count)
  {
  mAutoIndentSpaceCount = count;
  for( int i = 0; i < mEditorTab->count(); ++i ) {
    TextEditor *editor = getEditor(i);
    if( editor != nullptr )
      editor->setAutoIndentSpaceCount(mAutoIndentSpaceCount);
    }
  }




bool WCModeEditor::autoCompleteParenthesis() const
  {
  return mAutoCompleteParenthesis;
  }




void WCModeEditor::setAutoCompleteParenthesis(bool autoComplete)
  {
  mAutoCompleteParenthesis = autoComplete;
  for( int i = 0; i < mEditorTab->count(); ++i ) {
    TextEditor *editor = getEditor(i);
    if( editor != nullptr )
      editor->setAutoCompleteParenthesis(mAutoCompleteParenthesis);
    }
  }




//Активировать осциллограф
void WCModeEditor::oscillographActivate()
  {
  //Пройти по всем вкладкам, искать осциллограф
  for( int i = 0; i < mEditorTab->count(); i++ )
    if( dynamic_cast<WOscillograph*>( mEditorTab->widget(i) ) != nullptr ) {
      //Активировать найденный осциллограф
      mEditorTab->setCurrentIndex(i);
      return;
      }
  //Среди вкладок осциллограф не найден, вставить
  WOscillograph *osc = new WOscillograph( mDebugVar );
  int i = mEditorTab->addTab( osc, tr("Oscillograph") );
  mEditorTab->setTabToolTip( i, tr("Oscillograph window") );
  mEditorTab->setCurrentIndex( i );
  }


