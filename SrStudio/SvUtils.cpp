/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvUtils.h"
#include <QJsonValue>
#include <QJsonArray>

//Найти строку в списке без учета регистра
int findInList(const QString &pat, const QStringList &list)
  {
  for( int i = 0; i < list.count(); ++i )
    if( list.at(i).compare( pat, Qt::CaseInsensitive ) == 0 )
      return i;
  return -1;
  }



//Записать список строк
QJsonValue svStringListToValue(const QStringList &list)
  {
  return QJsonValue( QJsonArray::fromStringList(list) );
  }



//Прочитать список строк
QStringList svValueToStringList(QJsonValue val)
  {
  QJsonArray arr = val.toArray();
  QStringList list;
  int c = arr.count();
  for( int i = 0; i < c; i++ )
    list.append( arr.at(i).toString() );
  return list;
  }
