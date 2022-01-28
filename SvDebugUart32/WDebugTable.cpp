#include "WDebugTable.h"
#include "WOscillograph.h"

#include <QDebug>
#include <QRegExp>
#include <QVariantList>
#include <QJsonArray>
#include <QJsonObject>

#define DE_VAR_NAME  0 //Имя переменной
#define DE_VAR_VALUE 1 //Значение переменной
#define DE_VAR_FORM  2 //Формат значения
#define DE_VAR_ADDR  3 //Адрес
#define DE_VAR_AXIZ  4 //Включение-отключение отображения графиков 0-отключен, положительные числа-номера цветов из стандартной таблицы
#define DE_VAR_MIN   5 //Минимальное значение для масштабирования
#define DE_VAR_MAX   6 //Максимальное значение для масштабирования
#define DE_VAR_EDGE  7 //Контрольное значение из архива
#define DE_VAR_LAST  8 //Количество полей








WDebugTable::WDebugTable(QWidget *parent):
  QTableWidget(SV_MAX_DEBUG_VARS, DE_VAR_LAST, parent),
  mVarChangeLock(false),    //запрет на изменение значений
  mCurrentRow(0)
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

  //Настроить таймер для опроса переменных
  connect( &mTimer, &QTimer::timeout, this, &WDebugTable::onTimer );
  mTimer.start( 20 );
  }






QJsonArray WDebugTable::storeToProject()
  {
  //Сохранить переменные отладчика в проекте
  QJsonArray vars;
  for( int i = 0; i < rowCount(); i++ ) {
    QJsonObject var;
    var.insert( QStringLiteral("name"), getName(i) );
    var.insert( QStringLiteral("form"), getForm(i) );
    var.insert( QStringLiteral("axiz"), getAxiz(i) );
    var.insert( QStringLiteral("min"),  getMin(i) );
    var.insert( QStringLiteral("max"),  getMax(i) );
    vars.append( var );
    }

  //Сохранить список в проекте
  return vars;
  }






void WDebugTable::loadFromProject(const QJsonArray vars)
  {
  //Заполнить список переменных для отладки
  if( vars.count() == rowCount() ) {
    //Новый расширенный список переменных, здесь-же определяется и способ отображения на графике
    for( int i = 0; i < rowCount(); i++ ) {
      QJsonObject var = vars.at(i).toObject();
      setVariable(i, var.value(QStringLiteral("name")).toString(),
                     var.value(QStringLiteral("form")).toString(),
                     var.value(QStringLiteral("axiz")).toString(),
                     var.value(QStringLiteral("min")).toString(),
                     var.value(QStringLiteral("max")).toString() );
      }
    }
  else {
    //Просто очистить список
    clearVariables();
    }
  }









static auto nameParse( QString varName ) {
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
      //если не получилось расчитать - индекс не учитывать
      arrayIndex = 0;
      }
    //название перменной остается слева от индекса
    varName = varName.left(leftBracketIndex);
    }
  return std::make_tuple( varName, arrayIndex );
  }




void WDebugTable::debugVariable(int row, int column)
  {
  if( mVarChangeLock ) return;
  if( column == DE_VAR_NAME ) {
    //Изменилось имя
    //Имя убрали, стереть адрес, значение и формат
    item(row,DE_VAR_VALUE)->setText( QString() );
    item(row,DE_VAR_ADDR)->setText( QString() );
    item(row,DE_VAR_MIN)->setText( QString() );
    item(row,DE_VAR_MAX)->setText( QString() );

    QString varName = item(row,DE_VAR_NAME)->text();
    if( varName.isEmpty() ) {
      item(row,DE_VAR_FORM)->setText( QString() );
      item(row,DE_VAR_AXIZ)->setText( QString() );
      }
    }
  else if( column == DE_VAR_VALUE ) {
    //Введено новое значение для переменной
    int value = 0;
    QString val = item(row,DE_VAR_VALUE)->text();
    QString varName = item(row,DE_VAR_NAME)->text();
    if (varName.length() == 0)
      return;
    int arrayIndex = 0;
    std::tie( varName, arrayIndex ) = nameParse( varName );

    if( val.startsWith( QString("0x")) )
      value = val.mid(2).toInt(nullptr,16);
    else if( val.startsWith( QString("0b")) )
      value = val.mid(2).toInt(nullptr,2);
    else
      value = val.toInt();

    //Отправить запрос на изменение переменной
    emit setVariableValue( varName, arrayIndex, value );
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





void WDebugTable::onTimer()
  {
  // mCurrentRow считаем обработанным
  for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ ) {

    //Переходим к следующему ряду
    mCurrentRow++;

    //Обеспечим заворачивание
    if( mCurrentRow >= SV_MAX_DEBUG_VARS )
      mCurrentRow = 0;

    //Проверим наличие переменной для данного ряда
    QString varName = item( mCurrentRow, DE_VAR_NAME)->text();
    if( varName.isEmpty() ) {
      //нет названия или выражения - сбросить адрес
      item( mCurrentRow,DE_VAR_ADDR)->setText( QString("") );
      continue;
      }

    //Если какая либо ячейка в данном ряду редактируется, то пропускаем данную переменную
    bool edit = false;
    for( int i = 0; i < DE_VAR_LAST && !edit; i++ )
      if( isPersistentEditorOpen( model()->index( mCurrentRow, i ) ) )
        edit = true;

    if( edit ) continue;

    int arrayIndex = 0;

    std::tie(varName, arrayIndex) = nameParse( varName );
    emit queryVariable( mCurrentRow, varName, arrayIndex );
    return;
    }
  }






void WDebugTable::clearVariables()
  {
  //Стереть адреса и значения
  for( int i = 0; i < rowCount(); i++ ) {
    item(i,DE_VAR_VALUE)->setText( QString() );
    item(i,DE_VAR_ADDR)->setText( QString() );
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




void WDebugTable::onVariableValue(int row, int addr, int value)
  {
  mVarChangeLock = true;
  item( row, DE_VAR_ADDR )->setText( QString::number( addr, 16 ) );

  //представить значение в нужной форме
  QString format = item(row,DE_VAR_FORM)->text();
  if( format == QString("h") )
    item(row,DE_VAR_VALUE)->setText( QString("0x") + QString::number(value,16) );
  else if( format == QString("b") )
    item(row,DE_VAR_VALUE)->setText( QString("0b") + QString::number(value,2) );
  else
    item(row,DE_VAR_VALUE)->setText( QString::number(value) );

  mValues[row] = value;
  mVarChangeLock = false;
  }




