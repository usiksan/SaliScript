#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QDateTime>
#include <QListWidget>
#include <QTimer>
#include <QScrollBar>
#include "WTextSearchPanel.h"

class LineNumberArea;

///
/// \brief The BlockNode class локальный блок для С, содержащий начало(строку и символ), конец, уровень вложенности в текстовом документе
/// используется для построения графа блоков, что затем может использоваться для правильного автоиндента
///
class BlockNode: public QObject{
    Q_OBJECT
public:
    int mStartLineNumber;   //строка начала блока
    int mEndLineNumber;     //строка окончания блока
    int mLevel;             //уровень вложенности
    int mStartIndex;        //начальный индекс
    int mEndIndex;          //конечный индекс
    bool mSingle;           //является ли блок одиночным - без {}, например if () i++;
public:
    explicit BlockNode(BlockNode* parent):QObject(parent){}
    //содержит ли блок строку
    bool containsLine(int lineNum)const { if (mSingle) return lineNum <= mEndLineNumber && lineNum > mStartLineNumber; else return lineNum < mEndLineNumber && lineNum > mStartLineNumber; }
    //содержит ли блок символ
    bool containsIndex(int index)const { return index <= mEndIndex && index >= mStartIndex; }
};


///
/// \brief The ScrollBarWithMarkers class полоса прокрутки с метками поиска
/// mHighlightedLines - список <начало, высота>, значения от 0 до 1 (нормализовано по размеру документа в пискелях)
///
class ScrollBarWithMarkers : public QScrollBar {
    Q_OBJECT

    QList<QPair<double, double>> mHighlightedLines;
    int    mSelected;
    QColor mMarkerColor;
    QColor mSelectedMarkerColor;
protected:

    virtual void paintEvent(QPaintEvent *event) override;
public:
    explicit ScrollBarWithMarkers(QWidget* parent);
    void setHighlights(const  QList<QPair<double, double>>& positions, int selected);
};





//Текстовый редактор
class TextEditor : public QPlainTextEdit {
    Q_OBJECT

    //корень графа локальных блоков
    BlockNode* mRootBlock;
    //все локальные блоки для облегчения поиска
    QList<BlockNode*> mAllBlocks;

    ScrollBarWithMarkers* mScrollBarMarker;

    int             mAutoIndentSpacesCount;   //!< количество пробелов для автоотступа

    bool            mLastPressedReturn;
    bool            mAutoCompleteParenthesis;

    int             mDebugLine;          //!< Номер строки, в которой произведена остановка
    int             mBreakLine;          //!< Номер строки, в которой ожидается остановка
    QDateTime       mFileTime;           //!< Возраст файла
    QListWidget    *mAutoComplete;       //!< Окно списка автозавершения
    int             mLeftViewPortMargin; //!< Отступ слева для номеров строк
    bool            mIsUndoAvailable;
    bool            mIsRedoAvailable;
    bool            mIsSelectPresent;

    bool            mControlPress;       //!< Флаг нажатия кнопки Control
    QString         mLink;
    QTextBlock      mLinkBlock;
    QString         mOverWord;           //!< Идентификатор, над которым находится курсор

    QPlainTextEdit *mHelpPopUp;          //!< Окно всплывающей помощи
    QTimer          mHelpTime;           //!< Таймер, обеспечивающий появление помощи к идентификатору


   QList<QTextEdit::ExtraSelection> mSearchHighlight; //!< подстветка поиска

   QList<QTextEdit::ExtraSelection> mParenthesisHighlight; //!< подсветка скобок

   QTextEdit::ExtraSelection        mCurrentLineHighlight; //!< подстветка текущей строки


  public:
    QString         mFilePath;           //!< Полный путь к файлу

    explicit TextEditor(QWidget *parent = 0);
    virtual ~TextEditor();

    int debugLine() const { return mDebugLine; }
    int breakLine() const { return mBreakLine; }

    void setFileTime( const QDateTime &t ) { mFileTime = t; }
    bool isOlder( const QDateTime &t ) { return mFileTime < t; }


    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

    //Получить (идентификатор) слово под курсором
    QString getWordCursorOver();
    QString getWordCursorOver( QTextCursor cursor );

    //Получить файл из include под курсором
    QString getIncludeOver();

    //Получить директорий файла
    QString getFileDirectory();

    //Получить состояние редактора
    bool    isUndoAvailable() const { return mIsUndoAvailable; }
    bool    isRedoAvailable() const { return mIsRedoAvailable; }
    bool    isSelectPresent() const { return mIsSelectPresent; }

    int     autoIndentSpaceCount()const;
    void    setAutoIndentSpaceCount(int count);
    bool    autoCompleteParenthesis()const;
    void    setAutoCompleteParenthesis(bool autoComplete);

  private:
    //обновить подсветку
    void       updateHighlights();

    //количество пробелов в начале строки
    int        getLeadingSpaceCount(const QString &text);

    //определть в каком блоке находится строка
    BlockNode *getBlockForLine(int lineNum)const;

    //определить в каком блоке находится символ
    BlockNode *getBlockForIndex(int index)const;

    //Виджет отображающий номера строк
    QWidget  *lineNumberArea;

    //Автоматическая вставка отступов
    void      autoIndent();

    //Автоматическое завершение }
    bool      completeParenthesis();

    //Автозавершение ввода
    void      autoComplete();

    //Перезаполнить список автозавершения
    void      refillAutoComplete();

    //Получить часть слова, расположенную слева от курсора
    QString   getWordLeft();

    //Переключить комментарии
    void      toggleRemarkLine( QTextCursor tc );
    void      toggleRemark();

    //Реакция на нажатие F2
    void      toggleF2();

    //определить начало и конец блока для выделения скобок. Возвращает пару индексов символов документа <начало, конец>
    QPair<int, int> getParenthesisBlock(const QString & text, int pos)const;

    //закомментирован ли символ
    bool      isCommented(int index)const;

  private slots:
    void updateLineNumberAreaWidth(int newBlockCount);

    //подсветка текущей строки
    void highlightCurrentLine();
    //подсветка текущего блока
    void highlightCurrentBlock();

    void updateLineNumberArea(const QRect &rect, int dy);

    void onUndoAvailable( bool ena ) { mIsUndoAvailable = ena; }
    void onRedoAvailable( bool ena ) { mIsRedoAvailable = ena; }
    void onSelection( bool ena ) { mIsSelectPresent = ena; }

    //обновить граф локальных блоков
    void updateBlockGraph();

  protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mousePressEvent(QMouseEvent *e);

     void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

  signals:
    void rehighlightBlock( const QTextBlock block );
    void contensChanged();
    void fileOpen( const QString &fname );
    void setLink( const QString link );
    void jump( const QString fname, int line );

 public slots:
    void setDebugLine( int line );
    void onCursorPositionChanged();
    void locateLine( int line );
    //подсветка результатов поиска
    void highlightSearchResults(const QList<TextSearchResults> & searchResults, int currentIndex = -1);
    //замена текста с индекса startIndex по endIndex
    void replace(int startIndex, int endIndex, const QString & text);
    //замена текста по результатам поиска
    void replaceAll(const QList<TextSearchResults> & searchResults, const QString & text);
    //автоотступ строк, содержащихся между позициями по символам startIndex и endIndex
    void autoIndent(int startIndex, int endIndex, bool registerUndo = true);
    //автоотступ строки
    void autoIndentLine(int lineNum, bool registerUndo = false);
    //отступ строки на заданное количество пробелов
    void indentLine(int lineNum, int spacesCount, bool registerUndo = false);
    //Сгенерировать Sv QML код на основе выделения
    void editSvQmlGenerate();
    //void editSvQmlGenerateLine( const QString id, QStringList &result );
  };


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(TextEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    TextEditor *codeEditor;
};

#endif // TEXTEDITOR_H
