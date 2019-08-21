#ifndef WTEXTSEARCHPANEL_H
#define WTEXTSEARCHPANEL_H

#include <QCheckBox>
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>


///
/// \brief The TextSearchResults struct результат поиска текста
///
struct TextSearchResults{
    QString mFile;         //! файл, в котором найден текст
    int     mStartIndex;   //! индекс первого символа совпадения
    int     mEndIndex;     //! индекс последнего символа совпадения
    int     mLine;         //! номер строки
    QString mWord;         //! слово целиком
    TextSearchResults(const QString & file, int startIndex, int endIndex, int line, const QString & word):mFile(file),
        mStartIndex(startIndex),
        mEndIndex(endIndex),
        mLine(line),
        mWord(word){}
};

///
/// \brief The WTextSearchPanel class панель поиска и замены текста
/// Поиск и подстветка результатов производится в редакторе при обработке сигналов от объекта этого класса.
///
class WTextSearchPanel : public QDockWidget
{
    QCheckBox*         mCaseSensitiveCheckbox;  //! переключатель чувствительности регистра
    QCheckBox*         mFullWordCheckbox;       //! переключатель поиска слова целиком
    QLineEdit*         mSearchField;            //! строка поиска
    QPushButton*       mNextButton;             //! кнопка далее
    QPushButton*       mPrevButton;             //! кнопка назад

    QWidget*           mReplaceLine;            //! элементы замены текста
    QLineEdit*         mReplaceField;           //! строка замены
    QPushButton*       mReplaceButton;          //! кнопка заменить
    QPushButton*       mReplaceAllButton;       //! кнопка заменить все

    QListWidget*       mSearchResultTable;      //! таблица результатов поиска

    bool mSearchResultsValid;              //! параметры поиска изменились, так что по нажатию enter будет выполнен поиск, а не переход к следующему результату

    QList<TextSearchResults> mSearchResults;    //! результаты поиска
    QString                  mSearchedString;   //! искомая строка
    Q_OBJECT
protected:
    virtual void 	keyPressEvent(QKeyEvent * event)override;
    void searchValidate();
private slots:
    //изменение текста поля поиска
    void onSearchFieldTextChanged(const QString &  text);
    //нажат enter в поле поиска
    void onSearchFieldReturn();
    //показан/скрыт
    void onVisibleChanged(bool visible);
    //выбрана строка результатов
    void onResultsCurrentRowChanged(int row);
    //нажатие кнопок
    void onNextPressed();
    void onPrevPressed();
    void onReplacePressed();
    void onReplaceAllPressed();
    //изменены параметры поиска
    void onSearchParametersChanged();
    //нажат enter в поле замены
    void onReplaceFieldReturn();


public:
    explicit WTextSearchPanel(QWidget *parent = 0);

    //задать результаты поиска
    void setResults(const QList<TextSearchResults> & results);
    bool isCaseSensitive()const;
    bool isFullWord()const;

    //получение результата поиска
    TextSearchResults result(int index)const;
    //получение всех результатов поиска
    QList<TextSearchResults>  getResults()const { return mSearchResults; }
    //показать только поиск
    void showSearch();
    //показать поиск и замену
    void showReplace();

    //при нажатии enter в поле поиска поиска будет произведен новый поиск, а не переход к следующему
    void invalidateSearchResults();
signals:
    void wantSearch(const QString &  text);
    void wantHighglight(const QList<TextSearchResults> & results);
    void resultSelected(int index);

    void replace(const QString & replaceText, const TextSearchResults & searchResult);
    void replaceAll(const QString & replaceText, const QList<TextSearchResults> & results );
public slots:
};

#endif // WTEXTSEARCHPANEL_H
