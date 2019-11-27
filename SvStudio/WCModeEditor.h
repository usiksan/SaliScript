/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Текстовый редактор состоит из трех колонок:
      - колонка 1, здесь две ячейки:
        - перечень файлов проекта
        - перечень узлов сети с состояниями
      - колонка 2, здесь табулятор с редакторами
      - колонка 3, здесь две ячейки:
        - табулятор из следующих окон
          - окно с подсказкой
          - окно поиска и замены
          - окно с ошибками компилятора
          - окно с перечнем задач
        - таблица отлаживаемых переменных
*/

#ifndef CMODEEDITOR_H
#define CMODEEDITOR_H

#include "WDebugTable.h"
#include "WTextSearchPanel.h"
#include "SvHost/SvProgramm.h"
#include "SvHost/SvMirrorManager.h"

#include <QWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QTextBrowser>

class TextEditor;
class SvPeriodicParser;
class WMain;

#define SV_MAX_DEBUG_VARS 64 //Максимальное количество переменный (адресов) для мониторинга
#define SV_MAX_TASK       32 //Максимальное количество задач в узле


class WCModeEditor : public QSplitter
  {
    Q_OBJECT

    QString           mMainProjectFile;
    //Колонка 1
    QTabWidget       *mEditorTab;       //Список файлов для редактирования
    QLabel           *mTextStatus;      //Текстовое состояние подключения

    WTextSearchPanel *mSearchPanel;

    //Колонка 2
    QSplitter        *mDebugSplitter;   //Хранитель ячеек в колонке 3
    //Ячейка с инструментами
    QTabWidget       *mToolTab;         //Переключатель инструментов
      //Страница с файлами проекта
      QListWidget      *mProjectList;     //Список файлов проекта (формируется автоматически)
      //Страница с ошибками
      QListWidget      *mErrors;          //!< Окно со списком ошибок
      int               mErrorLine;       //!< Номер строки с текущей ошибкой
      //Страница с помощью
      QTextBrowser     *mHelp;            //Окно помощи
      //Страница со списком задач
      QTableWidget     *mTasks;           //Список задач
      //Страница с log-выводом
      QListWidget      *mLogList;         //Окно вывода log информации

    //Ячейка с отлаживаемыми переменными
    WDebugTable        *mDebugVar;        //Окно со списком мониторенных переменных

    SvPeriodicParser  *mParser;          //Периодический разборщик для обеспечения адекватной подсветки синтаксиса
    SvErrorList        mErrorList;       //Список ошибок

    bool               mVarChangeLock;   //Блокировка от изменения ячейки внутренними средствами
    QStringList        mFilesList;

    SvMirrorManager   *mManager;         //Менеджер зеркал

    bool               mAutoCompleteParenthesis;
    int                mAutoIndentSpaceCount;
  public:
    explicit WCModeEditor( SvMirrorManager *manager, WMain *parent );
    ~WCModeEditor();

    QStringList getFilesList()const { return mFilesList; }
    QString     getMainFileName()const  { return mMainProjectFile; }

    QByteArray  stateSplitterDebug() const { return mDebugSplitter->saveState(); }
    void        restoreSplitterDebug( const QByteArray & state ) { mDebugSplitter->restoreState( state ); }
    void        storeToProject();
    void        openFromProject();
    void        updateCommand();
    void        updateRecentFiles();
    QString     getEditorText( const QString & fname, bool &present );
    void        updateProjectFileList( const QStringList & list);
    void        parsingComplete();
    void        setParser( SvPeriodicParser *parser ) { mParser = parser; }

    //Возвращает истину если нету открытых файлов
    bool        isAllClosed() { return mEditorTab->count() == 0; }

    //Обновить связи отладчика
    void        connectVars( bool link );

  signals:
    void    compile();

  public slots:
    //Выполняет открытие файла в редакторе и обновляет список последних файлов
    void fileOpen( const QString & fname );
    void fileOpenByIndex( int index );
    void fileSave();
    void fileSaveAs();
    void fileSaveAll();
    void fileClose();
    void fileCloseAll();
    //void fileNew();

    void editUndo();
    void editRedo();
    void editCopy();
    void editCut();
    void editPaste();
    void editSelectAll();
    void editSearchText();
    void editReplaceText();
    void editAutoIndent();
    void editSvQmlGenerate();

    //Сохранить файл во вкладке с заданным индексом
    void fileSaveIndex( int tabIndex );

    //Закрыть вкладку с заданным индексом
    void fileCloseIndex( int tabIndex );

    //При изменении вкладки с редактором
    void onEditorIndex( int tabIndex );

    //При изменении undo, redo
    void onUndoRedoChanged( bool );

    //При изменении выделения
    void onSelectChanged(bool);

    //При изменении текста
    void textChanged();


    //при изменении зеркала
    void setupMirror(SvMirror* pMirror);

    //Установить новый список ошибок
    void setErrors( const SvErrorList list );

    //Изменилась позиция ошибки в списке ошибок
    void trackToError( int errorIndex );

    //Перейти к заданному файлу и строке
    void trackToFileLine( const QString & fname, int line );

    //Слоты для связи с отладчиком
    //При изменении задач
    void onTaskChanged();

    //При изменении памяти
   // void onMemoryChanged();

    //При поступлении loga
    void onLog(const QString msg );

    //Контекстная помощь
    void helpContext();

    //Добавить переменную для просмотра
    void debugAddWatch();

    //Пуск VPU
    void debugRun();

    //Пуск всех VPU
    void debugRunAll();

    //Шаг программы
    void debugStep();

    //Шаг программы с заходом в функцию
    void debugTrace();

    //Пауза VPU
    void debugPause();

    //Пауза всех VPU
    void debugPauseAll();

    //При изменении текстового статуса
    void onTextStatusChanged();


    int autoIndentSpaceCount()const;
    void setAutoIndentSpaceCount(int count);
    bool autoCompleteParenthesis()const;
    void setAutoCompleteParenthesis(bool autoComplete);

    //Активировать осциллограф
    void oscillographActivate();

  private:
    //Получить текстовый редактор с заданным индексом
    TextEditor *getEditor( int tabIndex );

    //Получить текущий текстовый редактор
    TextEditor *getCurrentEditor();

    //Получить идентификатор под курсором для текущего текстового редактора
    QString     getWordOverCursor();

    //Выполняет фактическое открытие файла в редакторе
    void        fileOpenInternal( const QString & fname );

    //Получить номер активной задачи
    int         task();


    void onSearchFieldWantSearch(const QString &  text);
    void onSearchFieldWantHighlight(const  QList<TextSearchResults> & results);
    void onSearchFieldIndexChanged(int index);
    void onSearchReplace(const QString & text, const TextSearchResults & searchResult);
    void onSearchReplaceAll(const QString & text, const QList<TextSearchResults> & results);
    QList<TextSearchResults> searchTextCurrentEditor(const QString & text, bool matchCase, bool fullWord);



  };

#endif // CMODEEDITOR_H
