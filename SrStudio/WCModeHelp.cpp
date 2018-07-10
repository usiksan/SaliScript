/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "WCModeHelp.h"
#include <QUrl>
#include <QSettings>
#include <QFile>

//Текущая библиотека
QString WCModeHelp::mCurrentLib;

WCModeHelp::WCModeHelp(QWidget *parent) : QTextBrowser(parent)
  {
  contens();
  }

WCModeHelp::~WCModeHelp()
  {

  }

//Преобразование названия страницы в фактический URL в соответствии с выбранным языком
QUrl WCModeHelp::pageConvert(const QString &page, const QString &fragment )
  {
  QSettings s;
  //Язык интерфейса
  QString lang = s.value( CFG_LANGUAGE, QVariant(QString("en")) ).toString();

  //Проверить наличие префикса
  if( page.contains( QChar('-') ) ) {
    //Cтраница с перенаправлением
    if( page.startsWith( QString("help-"), Qt::CaseInsensitive ) ) {
      //Перенаправление на стандартную помощь
      mCurrentLib = PATH_HELP;

      //Сформировать имя
      QString fname = page.mid( 5 );
      return pageConvert( fname, fragment );
      }

    //Перенаправление на помощь из библиотеки

    //Выделить префикс библиотеки
    int i = page.indexOf( QChar('-') );
    QString lib = page.left(i);

    //Сформировать имя
    QString fname = page.mid( i+1 );

    //Найти префикс библиотеки среди доступных библиотек
    QStringList list = s.value( CFG_LIBRARY_LIST, QVariant(QStringList()) ).toStringList();
    foreach( QString str, list ) {
      if( str.contains( lib, Qt::CaseInsensitive ) ) {
        //Проверить наличие файла с языком
        if( QFile::exists( str + "/" + lang + "_" + fname ) ) {
          mCurrentLib = str + "/";
          QUrl url = QUrl::fromLocalFile(str + "/" + lang + "_" + fname);
          if( !fragment.isEmpty() )
            url.setFragment( fragment );
          return url;
          }
        //Проверить наличие файла с анлийским языком
        else if( QFile::exists( str + "/en_" + fname ) ) {
          mCurrentLib = str + "/";
          QUrl url = QUrl::fromLocalFile(str + "/en_" + fname);
          if( !fragment.isEmpty() )
            url.setFragment( fragment );
          return url;
          }
        //Никакого файла нету, выдать файл с ошибкой
        return pageError();
        }
      }

    //Среди доступных библиотек нету
    return pageError();
    }

  //Файл помощи из теущего каталога

  //Проверить наличие файла на языке пользователя
  if( QFile::exists( mCurrentLib + lang + "_" + page ) ) {
    QUrl url = QUrl::fromLocalFile( mCurrentLib + lang + "_" + page );
    if( !fragment.isEmpty() )
      url.setFragment( fragment );
    return url;
    }
  //Проверить наличие файла с анлийским языком
  else if( QFile::exists( mCurrentLib + "en_" + page ) ) {
    QUrl url = QUrl::fromLocalFile( mCurrentLib + "en_" + page );
    if( !fragment.isEmpty() )
      url.setFragment( fragment );
    return url;
    }
  //Никакого файла нету, выдать файл с ошибкой
  return pageError();
  }





//Вернуть страницу помощи с ошибкой ссылки
QUrl WCModeHelp::pageError()
  {
  QSettings s;
  //Язык интерфейса
  QString lang = s.value( CFG_LANGUAGE, QVariant(QString("en")) ).toString();
  //Проверить наличие страницы с ошибкой на языке пользователя
  if( QFile::exists( QString(PATH_HELP) + lang + "_errorNoPage.htm" ) )
    return QUrl::fromLocalFile( QString(PATH_HELP) + lang + "_errorNoPage.htm" );
  //Вернуть страницу с ошибкой на английском
  return QUrl::fromLocalFile( QString(PATH_HELP) + "en_errorNoPage.htm" );
  }






//Выдать домашнюю страницу относительно текущей библиотеки
QUrl WCModeHelp::pageLibraryHead()
  {
  //Пустой каталог текущей библиотеки означает глобальную помощь
  if( mCurrentLib.isEmpty() )
    mCurrentLib = PATH_HELP;

  return pageConvert( "ahead.htm", QString() );
  }

QUrl WCModeHelp::pageHead()
  {
  return pageConvert( "help-ahead.htm", QString() );
  }






void WCModeHelp::contens()
  {
  //Выдать начальную страницу
  setSource( pageHead() );
  }

