/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef DSUTILS
#define DSUTILS
#include <QString>
#include <QStringList>
#include <QDataStream>


//Записать список строк
QJsonValue svStringListToValue( const QStringList &list );

//Прочитать список строк
QStringList svValueToStringList( QJsonValue val );


//Поиск строки в списке без учета регистра
int findInList( const QString &pat, const QStringList &list );


template<class T>
void write( QList<T*> list, QDataStream &os ) {
  qint32 count = list.count();
  os << count;
  foreach( T *ptr, list )
    ptr->write( os );
  }

template<class T>
void read( QList<T*> &list, QDataStream &is ) {
  qint32 count;
  is >> count;
  for( int i = 0; i < count; i++ ) {
    T *ptr = new T();
    ptr->read( is );
    list.append( ptr );
    }
  }

#endif // DSUTILS

