/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "WVisualLibrary.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


WVisualLibrary::WVisualLibrary(QWidget *parent) :
  QTextBrowser(parent)
  {
  connect( this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
  setHome();
  }






WVisualLibrary::~WVisualLibrary()
  {

  }






void WVisualLibrary::onLinkClicked(const QUrl &url)
  {
  QString str( url.toString( QUrl::None ) );
  if( str.startsWith(QString("append:")) ) appendLibrary();
  else if( str.startsWith(QString("del:")) ) removeLibrary( str.mid(4) );
  else if( str.startsWith(QString("info:")) ) emit preview( str.mid(5) );
  else if( str.startsWith(QString("dir:")) ) enterLibrary( str.mid(4) );
  else if( str.startsWith(QString("obj:")) ) emit apply( str.mid(4) );
  else if( str.startsWith(QString("cat:")) ) enterCategory( str.mid(4) );
  }






void WVisualLibrary::home()
  {
  setHome();
  }






void WVisualLibrary::backward()
  {

  }






void WVisualLibrary::parsingObjectFullList(QDir libDir)
  {
  //Удалить прежние категории
  qDeleteAll(mCategoryMap);
  mCategoryMap.clear();

  //Для всех файлов-объектов выполнить
  foreach( QFileInfo info, mObjectFullList ) {
    //Открыть файл
    QFile file( info.absoluteFilePath() );
    if( file.open(QIODevice::ReadOnly) ) {
      //Прочитать файл как json
      QJsonObject obj = QJsonDocument::fromJson( file.readAll() ).object();
      QJsonArray list = obj.value( "category" ).toArray();
      for( int i = 0; i < list.count(); i++ ) {
        DsLibCategory *cat = getCategory( libDir, list.at(i).toString().simplified() );
        if( cat )
          cat->mObjectList.append( info.absoluteFilePath() );
        }
      }
    }
  }






void WVisualLibrary::enterLibrary(QString lib)
  {
  qDebug() << "enterLib";
  //Входим в библиотеку
  mCurrentLibraryPath = lib + QString("/");

  //Образовать путь
  QDir libDir(lib);

  //Получить список всех объектов
  QStringList filter;
  filter << QString("*" EXTENSION_LIB);
  mObjectFullList = libDir.entryInfoList( filter, QDir::Files );
  parsingObjectFullList( libDir );

  mPath = QString("index");
  enterCategory( mPath );
  }






void WVisualLibrary::enterCategory(QString catalog)
  {
  //Выбрать категорию
  DsLibCategory *cat = mCategoryMap.value( catalog, 0 );
  if( cat ) {
    //Сформировать объект
    QString pg("<html><body><h1 align=\"center\">");
    pg.append( cat->mTitle ).append( "</h1>" );

    foreach( QString subCat, cat->mSubCategory ) {
      DsLibCategory *s = mCategoryMap.value( subCat, 0 );
      if( s ) {
        pg.append("<figure><p><a href=\"cat:").
           append(subCat).append("\"><img src=\"").append(s->mIcon).append("\"></p><figcaption>").
            append(s->mTitle).append("</figcaption></figure>");
        }
      }
    //Проверить файлы в категории
  //    if( cat->mObjectList.count() ) {
      pg.append("<hr>");
      pg.append( "<p><a href=\"obj:SmPin.h\">SmPin.h</a></p>" );
  //      foreach( QString obj, cat->mObjectList ) {

  //        }
  //    }
    //Закрыть страницу
    pg.append("</body></html>");
    setHtml( pg );
    }
  }






DsLibCategory *WVisualLibrary::getCategory(QDir libDir, const QString category)
  {
  //Проверить наличие категории
  if( mCategoryMap.contains(category) )
    return mCategoryMap.value(category,0);

  //Прямой категории нету, получить родительскую категорию
  int parentIndex = category.lastIndexOf( QChar('.') );
  if( parentIndex > 0 ) {
    DsLibCategory *parent = getCategory( libDir, category.left(parentIndex) );
    //Добавить подкатегорию
    parent->mSubCategory.append( category );
    }

  //Создать и сохранить категорию
  DsLibCategory *cat = new DsLibCategory( libDir, category, tr("No description file") );
  mCategoryMap.insert( category, cat );
  return cat;
  }






void WVisualLibrary::appendLibrary()
  {
  //Добавление нового директория к списку библиотек
  QString dir = QFileDialog::getExistingDirectory( this, tr("Select library directory") );
  if( !dir.isEmpty() ) {
    //Проверить наличие файла с описанием библиотеки
    QString about( dir );
    about.append( "/about_en.htm" );
    if( QFile::exists( about ) ) {
      //Добавить каталог в список библиотек
      QSettings s;
      QStringList list = s.value( CFG_LIBRARY_LIST, QVariant(QStringList()) ).toStringList();
      list.append( dir );
      s.setValue( CFG_LIBRARY_LIST, QVariant(list) );
      setHome();
      }
    else QMessageBox::warning( this, tr("Error!"), tr("Selected catalog is not a library. Select another.") );
    }
  }






void WVisualLibrary::removeLibrary(QString path)
  {
  //Выдать запрос подтверждения
  if( QMessageBox::question(this, tr("Warning"), tr("Are You sure remove library \"%1\" from library list?").arg(path) ) == QMessageBox::Yes ) {
    //Удалить каталог из списка
    QSettings s;
    QStringList list = s.value( CFG_LIBRARY_LIST, QVariant(QStringList()) ).toStringList();
    list.removeAll( path );
    s.setValue( CFG_LIBRARY_LIST, QVariant(list) );
    setHome();
    }
  }






QString WVisualLibrary::language()
  {
  QSettings s;
  return s.value( CFG_LANGUAGE, QVariant(QString("en")) ).toString();
  }






void WVisualLibrary::setHome()
  {
  QString pg("<html><body><h1 align=\"center\">");
  pg.append( tr("Library list") ).append( QString("</h1><p><button><img src=\"qrc:///pic/plus_blue.png\" align=\"middle\"> <a href=\"append:\">") );
  pg.append( tr("Append library to list") ).append( QString("</a></button></p><hr>") );

  //Добавить стандартную библиотеку, которую нельзя удалять
  pg.append( QString("<p><img src=\"qrc:///pic/directory_closed.png\" align=\"middle\"> <a href=\"dir:%1\">").arg(PATH_VISUAL) );
  pg.append( tr("Standard library") ).append("</a></p>");

  //Добавить список существующих библиотек
  QSettings s;
  QStringList list = s.value( CFG_LIBRARY_LIST, QVariant(QStringList()) ).toStringList();
  foreach( QString str, list ) {
    pg.append( QString("<p><a href=\"info:%1\"><img src=\"qrc:///pic/directory_closed.png\" align=\"middle\"> <img src=\"qrc:///pic/info32.png\" align=\"middle\"></a> <a href=\"dir:%1\">%1</a> <a href=\"del:%1\"><img src=\"qrc:///pic/delete_red.png\" align=\"middle\"></a></p>").arg(str) );
    }


  //Установить текущую страницу
  setHtml( pg );
  setOpenLinks(false);
  }

