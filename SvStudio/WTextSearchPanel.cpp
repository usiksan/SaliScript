#include "WTextSearchPanel.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QStringListModel>



WTextSearchPanel::WTextSearchPanel(QWidget *parent) : QDockWidget(parent),
  mSearchResultsValid(true)
  {
  QWidget* pContext = new QWidget();
  QVBoxLayout* pLayout = new QVBoxLayout();

  QWidget* pSearchLine = new QWidget();
  mSearchField = new QLineEdit();
  mNextButton = new QPushButton(tr("Next"));
  mPrevButton = new QPushButton(tr("Previous"));
  mCaseSensitiveCheckbox = new QCheckBox(tr("Case sensitive"));
  mFullWordCheckbox = new QCheckBox(tr("Full word"));
  QHBoxLayout* pSearchLineLayout = new QHBoxLayout();
  pSearchLineLayout->addWidget(mCaseSensitiveCheckbox);
  pSearchLineLayout->addWidget(mFullWordCheckbox);
  pSearchLineLayout->addWidget(mSearchField);
  pSearchLineLayout->addWidget(mPrevButton);
  pSearchLineLayout->addWidget(mNextButton);
  pSearchLine->setLayout(pSearchLineLayout);

  mReplaceLine = new QWidget();
  mReplaceField = new QLineEdit();
  mReplaceButton = new QPushButton(tr("Replace"));
  mReplaceAllButton = new QPushButton(tr("Replace all"));
  QHBoxLayout* pReplaceLineLayout = new QHBoxLayout();
  pReplaceLineLayout->addWidget(mReplaceField);
  pReplaceLineLayout->addWidget(mReplaceButton);
  pReplaceLineLayout->addWidget(mReplaceAllButton);
  mReplaceLine->setLayout(pReplaceLineLayout);

  //    mSearchResultLabel = new QLabel();
  mSearchResultTable = new QListWidget();
  //    pLayout->addWidget(mSearchResultLabel);
  pLayout->addWidget(pSearchLine);
  pLayout->addWidget(mReplaceLine);
  pLayout->addWidget(mSearchResultTable);

  pContext->setLayout(pLayout);
  setWidget(pContext);

  setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  connect(mSearchField, &QLineEdit::textChanged, this, &WTextSearchPanel::onSearchFieldTextChanged);
  connect(mSearchField, &QLineEdit::returnPressed, this, &WTextSearchPanel::onSearchFieldReturn);
  connect(mSearchResultTable, &QListWidget::currentRowChanged, this, &WTextSearchPanel::onResultsCurrentRowChanged);
  connect(mNextButton, &QPushButton::clicked, this, &WTextSearchPanel::onNextPressed);
  connect(mPrevButton, &QPushButton::clicked, this, &WTextSearchPanel::onPrevPressed);

  connect(mCaseSensitiveCheckbox, &QCheckBox::clicked, this, &WTextSearchPanel::onSearchParametersChanged);
  connect(mFullWordCheckbox, &QCheckBox::clicked, this, &WTextSearchPanel::onSearchParametersChanged);

  connect (mReplaceField, &QLineEdit::returnPressed, this, &WTextSearchPanel::onReplaceFieldReturn);
  connect(mReplaceButton, &QPushButton::pressed, this, &WTextSearchPanel::onReplacePressed);
  connect(mReplaceAllButton, &QPushButton::pressed, this, &WTextSearchPanel::onReplaceAllPressed);

  mNextButton->setEnabled(false);
  mPrevButton->setEnabled(false);

  connect(this, &WTextSearchPanel::visibilityChanged, this, &WTextSearchPanel::onVisibleChanged);

  }





//задать результаты поиска
void WTextSearchPanel::setResults(const QList<TextSearchResults> &results)
  {
  mSearchResults = results;
  mSearchResultTable->clear();
  //обновить содержимое таблицы  результатов
  foreach (auto result, mSearchResults) {
    auto str = result.mFile + ", " + tr("Line %1").arg(result.mLine) + ": " + result.mWord;
    mSearchResultTable->addItem(str);
    }

  //обновить заголовк окна
  setWindowTitle(tr("Found %1 results").arg(results.count()));
  if (results.count() > 0)
    mSearchResultTable->setCurrentRow(0);
  //включить кнопки если нужно
  mNextButton->setEnabled(mSearchResults.count() > 0);
  mPrevButton->setEnabled(mSearchResults.count() > 0);

  //сообщить, что нужна подсветка
  emit wantHighglight(mSearchResults);
  }




bool WTextSearchPanel::isCaseSensitive()const
  {
  return mCaseSensitiveCheckbox->isChecked();
  }





bool WTextSearchPanel::isFullWord() const
  {
  return mFullWordCheckbox->isChecked();
  }




TextSearchResults WTextSearchPanel::result(int index) const
  {
  if (index <0 || index >= mSearchResults.count())
    return TextSearchResults("", -1, -1, -1, "");
  return mSearchResults[index];
  }




void WTextSearchPanel::showSearch()
  {
  setVisible(true);
  mReplaceLine->setVisible(false);
  mSearchField->setFocus();
  mSearchField->selectAll();

  mSearchResultsValid = false;
  searchValidate();
  }





void WTextSearchPanel::showReplace()
  {
  setVisible(true);
  mReplaceLine->setVisible(true);
  mSearchField->setFocus();
  mSearchField->selectAll();
  mSearchResultsValid = false;
  searchValidate();
  }




void WTextSearchPanel::invalidateSearchResults()
  {
  mSearchResultsValid = false;
  }




void WTextSearchPanel::keyPressEvent(QKeyEvent *event)
  {
  if (event->key() == Qt::Key_Escape)
    setVisible(false);
  else
    QDockWidget::keyPressEvent(event);
  }




void WTextSearchPanel::searchValidate()
  {
  if (!mSearchResultsValid){
    mSearchedString = mSearchField->text();
    emit wantSearch(mSearchedString);
    mSearchResultsValid = true;
    }
  }




//изменение текста поля поиска
void WTextSearchPanel::onSearchFieldTextChanged(const QString &text)
  {
  Q_UNUSED(text)
  invalidateSearchResults();
  searchValidate();
  //mSearchedString = text;
  //emit wantSearch(mSearchedString);
  }




//нажат enter в поле поиска
void WTextSearchPanel::onSearchFieldReturn()
  {
  onNextPressed();
  }



//показан/скрыт
void WTextSearchPanel::onVisibleChanged(bool visible)
  {
  if (!visible)
    emit wantHighglight(QList<TextSearchResults>());
  }




//выбрана строка результатов
void WTextSearchPanel::onResultsCurrentRowChanged(int row)
  {
  emit resultSelected(row);
  }




void WTextSearchPanel::onNextPressed()
  {
  searchValidate();

  if (mSearchResultTable->currentRow() + 1 >= mSearchResultTable->count())
    mSearchResultTable->setCurrentRow(0);
  else
    mSearchResultTable->setCurrentRow(mSearchResultTable->currentRow() + 1);
  }




void WTextSearchPanel::onPrevPressed()
  {
  searchValidate();
  if (mSearchResultTable->currentRow() - 1 < 0)
    mSearchResultTable->setCurrentRow(mSearchResultTable->count() - 1);
  else
    mSearchResultTable->setCurrentRow(mSearchResultTable->currentRow() - 1);
  }




void WTextSearchPanel::onSearchParametersChanged()
  {
  invalidateSearchResults();
  }





//нажат enter в поле замены
void WTextSearchPanel::onReplaceFieldReturn()
  {
  //то же, что и нажатие кнопки заменить
  onReplacePressed();
  }




//нажатие кнопки заменить
void WTextSearchPanel::onReplacePressed()
  {
  searchValidate();
  int selectedIndex = mSearchResultTable->currentRow();
  if (selectedIndex >= 0 && selectedIndex < mSearchResults.count()){
    auto result = mSearchResults[selectedIndex];
    //заменить
    emit replace (mReplaceField->text(), result);
    //произвести новый поиск
    emit wantSearch(mSearchedString);
    //выбрать следующий элемент
    if (mSearchResultTable->count() > 0){
      if (selectedIndex < mSearchResultTable->count())
        mSearchResultTable->setCurrentRow(selectedIndex);
      else
        mSearchResultTable->setCurrentRow(0);
      }
    }

  }




void WTextSearchPanel::onReplaceAllPressed()
  {
  searchValidate();
  emit replaceAll(mReplaceField->text(), mSearchResults);
  emit wantSearch(mSearchedString);
  }
