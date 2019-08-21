/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "DsLibCategory.h"
#include <QSettings>

DsLibCategory::DsLibCategory( QDir libDir, const QString categoryName, const QString noDescription )
  {
  QSettings s;
  QString language = s.value( CFG_LANGUAGE, QString("en") ).toString();
  //Заполнить заголовок по умолчанию
  mTitle = categoryName;
  //Получить заголовок
  if( libDir.exists( categoryName + QString(".title") ) ) {
    //Файл с заголовком существует, прочитать его и выбрать согласно языка пользователя
    QFile file( libDir.filePath( categoryName + QString(".title") ) );
    if( file.open(QIODevice::ReadOnly) ) {
      //Прочитать файл как массив строк
      QStringList list = QString::fromUtf8( file.readAll() ).split(QChar('\n'));
      foreach( QString line, list ) {
        //Если строка на английском, то установить ее на случай отсутствия перевода
        if( line.startsWith( QString("en") ) )
          mTitle = line.mid(3);
        //Если перевод найден, то установить и закончить поиск
        if( line.startsWith( language ) ) {
          mTitle = line.mid(3);
          break;
          }
        }
      }
    }

  //Установить описание по умолчанию
  mDescription = noDescription;
  //Получить файл с описанием
  if( libDir.exists( categoryName + QString("_") + language + QString(".htm") ) ) {
    //Файл с описанием есть, прочитать
    QFile file( categoryName + QString("_") + language + QString(".htm") );
    if( file.open(QIODevice::ReadOnly) )
      mDescription = QString::fromUtf8( file.readAll() );
    }
  else if( libDir.exists( categoryName + QString("_en.htm") ) ) {
    //Файла с описанием для языка пользователя нету, прочитать файл на английском
    QFile file( categoryName + QString("_") + language + QString(".htm") );
    if( file.open(QIODevice::ReadOnly) )
      mDescription = QString::fromUtf8( file.readAll() );
    }

  }

DsLibCategory::~DsLibCategory()
  {

  }

