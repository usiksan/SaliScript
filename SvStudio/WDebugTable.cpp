#include "WDebugTable.h"
#include "SvDebugThread.h"
#include "IngDebugCalculator.h"
#include "WOscillograph.h"
#include <QDebug>
#include <QRegExp>
#include <QVariantList>

#define DE_VAR_NAME  0 //Имя переменной
#define DE_VAR_VALUE 1 //Значение переменной
#define DE_VAR_FORM  2 //Формат значения
#define DE_VAR_ADDR  3 //Адрес
#define DE_VAR_AXIZ  4 //Включение-отключение отображения графиков 0-отключен, положительные числа-номера цветов из стандартной таблицы
#define DE_VAR_MIN   5 //Минимальное значение для масштабирования
#define DE_VAR_MAX   6 //Максимальное значение для масштабирования
#define DE_VAR_EDGE  7 //Контрольное значение из архива
#define DE_VAR_LAST  8 //Количество полей








WDebugTable::WDebugTable(QWidget *parent): QTableWidget(SV_MAX_DEBUG_VARS, DE_VAR_LAST, parent),
  mMirror(nullptr),         //зеркало, с которого берутся значения
  mVarChangeLock(false),    //запрет на изменение значений
  mDebugCalculator(nullptr) //расчет выражений
  {

  QStringList debugTitles;
  debugTitles << tr("Variable name") << tr("Value") << tr("Form") << tr("Address") << tr("Axiz") << tr("Min") << tr("Max") << tr("Edge");
  setHorizontalHeaderLabels(debugTitles);
  //Заполнить таблицу элементами
  for( int i = 0; i < SV_MAX_DEBUG_VARS; ++i ) {
    setItem( i, DE_VAR_NAME, new QTableWidgetItem() );
    setItem( i, DE_VAR_VALUE, new QTableWidgetItem() );
    setItem( i, DE_VAR_FORM, new QTableWidgetItem() );
    setItem( i, DE_VAR_ADDR, new QTableWidgetItem() );
    setItem( i, DE_VAR_AXIZ, new QTableWidgetItem() );
    setItem( i, DE_VAR_MIN, new QTableWidgetItem() );
    setItem( i, DE_VAR_MAX, new QTableWidgetItem() );
    setItem( i, DE_VAR_EDGE, new QTableWidgetItem() );
    setRowHeight( i, 20 );
    mValues[i] = 0;
    }
  setColumnWidth( DE_VAR_FORM, 50 );
  setColumnWidth( DE_VAR_ADDR, 50 );
  setColumnWidth( DE_VAR_AXIZ, 50 );
  setColumnWidth( DE_VAR_MIN, 50 );
  setColumnWidth( DE_VAR_MAX, 50 );
  setColumnWidth( DE_VAR_EDGE, 50 );

  connect( this, SIGNAL(cellChanged(int,int)), this, SLOT(debugVariable(int,int)) );


  mDebugCalculator = new IngDebugCalculator(new IngDebugMirrorVarProvider(mMirror), this);
  }






void WDebugTable::displayDebugValue(int row)
  {
  mVarChangeLock = true;
  int value = 0;
  int addr = item(row,DE_VAR_ADDR)->text().toInt();
  bool ok = false;
  if( mMirror ) {
    //если есть адрес - получить значение по нему
    if( addr ) {
      value = mMirror->memoryGet( addr );
      ok = true;
      }
    else{
      //нет адреса - расчитать выражение
      auto varExpression = item(row,DE_VAR_NAME)->text();
      value = mDebugCalculator->value(varExpression, ok);
      }
    }

  if( ok ) {
    //представить значение в нужной форме
    QString format = item(row,DE_VAR_FORM)->text();
    if( format == QString("h") )
      item(row,DE_VAR_VALUE)->setText( QString("0x") + QString::number(value,16) );
    else if( format == QString("b") )
      item(row,DE_VAR_VALUE)->setText( QString("0b") + QString::number(value,2) );
    else
      item(row,DE_VAR_VALUE)->setText( QString::number(value) );
    }
  else{
    item(row,DE_VAR_VALUE)->setText( QString() );
    }

  mValues[row] = value;
  mVarChangeLock = false;
  }




void WDebugTable::updateAddress(int row)
  {
  QString varName = item(row, DE_VAR_NAME)->text();
  if( varName.isEmpty() ) {
    //нет названия или выражения - сбросить адрес
    item(row,DE_VAR_ADDR)->setText( QString("") );
    return;
    }
  int arrayIndex = 0;

  int leftBracketIndex = varName.indexOf("[");
  int rightBracketIndex = varName.lastIndexOf("]");
  //найдены скобки для элемента массива
  if( leftBracketIndex >= 0 && rightBracketIndex >=0 ) {
    QString arrayIndexString = varName.mid(leftBracketIndex+1,
                                           rightBracketIndex - leftBracketIndex - 1);

    //если индекс можно сразу преобразовать к int - сделать это
    bool ok = false;
    arrayIndex = arrayIndexString.toInt(&ok);
    if (!ok){
      //если не получилось сразу к int, возможно там выражение - произвести расчет
      arrayIndex = mDebugCalculator->value(arrayIndexString, ok);
      //если не получилось расчитать - индекс не учитывать
      if (!ok)
        arrayIndex = 0;
      }
    //название перменной остается слева от индекса
    varName = varName.left(leftBracketIndex);
    }
  if( mMirror ) {
    if (mMirror->getLink()){
      int addr = mMirror->addressOfName(varName) + arrayIndex;
      //Установить новый адрес
      item(row,DE_VAR_ADDR)->setText( QString::number( addr ) );
      }
    else
      item(row,DE_VAR_ADDR)->setText( QString("") );
    }
  else {
    //Канал не установлен, адреса нету
    item(row,DE_VAR_ADDR)->setText( QString("") );
    }
  }




void WDebugTable::debugVariable(int row, int column)
  {
  if( mVarChangeLock ) return;
  if( column == DE_VAR_NAME ) {
    //Изменилось имя
    QString varName = item(row,DE_VAR_NAME)->text();
    if( varName.isEmpty() ) {
      //Имя убрали, стереть адрес, значение и формат
      item(row,DE_VAR_VALUE)->setText( QString() );
      item(row,DE_VAR_FORM)->setText( QString() );
      item(row,DE_VAR_ADDR)->setText( QString() );
      item(row,DE_VAR_AXIZ)->setText( QString() );
      item(row,DE_VAR_MIN)->setText( QString() );
      item(row,DE_VAR_MAX)->setText( QString() );
      }
    else {
      //Изменилось имя, получить новый адрес
      updateAddress(row);
      }
    //Получить значение
    displayDebugValue( row );
    }
  else if( column == DE_VAR_VALUE ) {
    //Введено новое значение для переменной
    int value;
    QString val = item(row,DE_VAR_VALUE)->text();
    QString varName = item(row,DE_VAR_NAME)->text();
    if (varName.length() == 0)
      return;
    int address = item(row,DE_VAR_ADDR)->text().toInt();
    if( address == 0 )
      return;
    if( val.startsWith( QString("0x")) )
      value = val.mid(2).toInt(nullptr,16);
    else if( val.startsWith( QString("0b")) )
      value = val.mid(2).toInt(nullptr,2);
    else
      value = val.toInt();

    //Отправить запрос на изменение переменной
    if( mMirror )
      mMirror->memorySet( address, value );
    }
  else if( column == DE_VAR_AXIZ ) {
    //Изменен номер оси, поменять цвет
    int iaxiz = item(row,DE_VAR_AXIZ)->text().toInt();
    if( iaxiz ) item(row, DE_VAR_AXIZ)->setBackgroundColor( WOscillograph::axizColor(iaxiz) );
    else item(row, DE_VAR_AXIZ)->setBackgroundColor( Qt::white );
    emit repaintOscillograph();
    }
  else if( column == DE_VAR_MIN || column == DE_VAR_MAX ) {
    emit repaintOscillograph();
    }
  }





void WDebugTable::onMemoryChanged()
  {
  if (sender() == nullptr){
    return;
    }

  updateVariables();
  }



void WDebugTable::onLinkStatusChanged(bool link, const QString status)
  {
  Q_UNUSED(link)
  Q_UNUSED(status)
  }










void WDebugTable::debugVariables()
  {
  for( int i = 0; i < rowCount(); i++ )
    debugVariable(i, DE_VAR_NAME);
  }



void WDebugTable::clearVariables()
  {
  //Стереть адреса и значения
  for( int i = 0; i < rowCount(); i++ ) {
    item(i,DE_VAR_VALUE)->setText( QString() );
    item(i,DE_VAR_ADDR)->setText( QString() );
    }

  }



void WDebugTable::updateVariables()
  {
  for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ ) {
    //Получить полный адрес переменной
    updateAddress(i);
    displayDebugValue( i );
    }
  }




QString WDebugTable::getName(int index)
  {
  return  item(index, DE_VAR_NAME)->text();
  }




QString WDebugTable::getForm(int index)
  {
  return  item(index, DE_VAR_FORM)->text();
  }




QString WDebugTable::getAxiz(int index)
  {
  return item(index, DE_VAR_AXIZ)->text();
  }




QString WDebugTable::getMin(int index)
  {
  return item(index, DE_VAR_MIN)->text();
  }




QString WDebugTable::getMax(int index)
  {
  return item(index, DE_VAR_MAX)->text();
  }




void WDebugTable::debugAppend(const QString & name)
  {
  if( name.isEmpty() )
    return;
  //Отыскать пустую ячейку и вставить туда переменную
  for( int i = 0; i < rowCount(); i++ )
    if( item(i,DE_VAR_NAME)->text().isEmpty() ) {
      //Можно вставлять
      item(i,DE_VAR_NAME)->setText( name );
      //Объявить, что переменная обновлена
      debugVariable( i, DE_VAR_NAME );
      return;
      }
  }




void WDebugTable::setVariable(int index, const QString name, const QString form, const QString axiz, const QString mmin, const QString mmax )
  {
  item(index, DE_VAR_NAME)->setText(name);
  item(index, DE_VAR_ADDR)->setText("");
  item(index, DE_VAR_FORM)->setText(form);
  item(index, DE_VAR_VALUE)->setText("");
  item(index, DE_VAR_AXIZ)->setText(axiz);
  int iaxiz = axiz.toInt();
  if( iaxiz ) item(index, DE_VAR_AXIZ)->setBackgroundColor( WOscillograph::axizColor(iaxiz) );
  else item(index, DE_VAR_AXIZ)->setBackgroundColor( Qt::white );
  item(index, DE_VAR_MIN)->setText(mmin);
  item(index, DE_VAR_MAX)->setText(mmax);
  item(index, DE_VAR_EDGE)->setText(QString());
  }




void WDebugTable::setEdgeValue(int row, int value)
  {
  //представить значение в нужной форме
  QString format = item(row,DE_VAR_FORM)->text();
  if( format == QString("h") )
    item(row,DE_VAR_EDGE)->setText( QString("0x") + QString::number(value,16) );
  else if( format == QString("b") )
    item(row,DE_VAR_EDGE)->setText( QString("0b") + QString::number(value,2) );
  else
    item(row,DE_VAR_EDGE)->setText( QString::number(value) );
  }




void WDebugTable::setupMirror(SvMirror *pNewMirror)
  {
  mMirror = pNewMirror;
  //подключить сигналы
  if( mMirror != nullptr ) {
    connect( mMirror, &SvMirror::memoryChanged, this, &WDebugTable::onMemoryChanged);
    connect( mMirror, &SvMirror::linkStatusChanged, this, &WDebugTable::onLinkStatusChanged);
    // connect( mMirror, &SvMirror::destroyed, this, &WDebugTable::resetMirror);
    onLinkStatusChanged(mMirror->getLink(), mMirror->getLinkStatus());
    }
  //задать зеркало в расчет значений
  auto pProvider = dynamic_cast<IngDebugMirrorVarProvider*>( mDebugCalculator->valueProvider());
  if( pProvider != nullptr )
    pProvider->setMirror(pNewMirror);
  }

