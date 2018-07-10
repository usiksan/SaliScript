/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef WVISUALLIBRARY_H
#define WVISUALLIBRARY_H

#include "DsLibCategory.h"
#include <QWidget>
#include <QFileInfoList>
#include <QStringList>
#include <QMap>
#include <QStack>
#include <QTextBrowser>



class WVisualLibrary : public QTextBrowser
  {
    Q_OBJECT

    QFileInfoList     mObjectFullList;     //Полный список объектов библиотеки
    DsLibCategoryMap  mCategoryMap;        //Карта категорий библиотеки
    QString           mCurrentLibraryPath; //Путь к файлам текущей библиотеки
    QStack<QString>   mHistory;            //История посещений
    QString           mPath;               //Текущий путь

  public:
    explicit WVisualLibrary(QWidget *parent = 0);
    ~WVisualLibrary();

  signals:
    void           preview( const QString &path );
    void           apply( const QString &path );

  public slots:
    void           onLinkClicked( const QUrl &url );
    void           home();
    void           backward();

  private:
    void           parsingObjectFullList(QDir libDir);
    void           enterLibrary( QString lib );
    void           enterCategory(QString catalog );
    DsLibCategory* getCategory( QDir libDir, const QString category );
    void           appendLibrary();
    void           removeLibrary( QString path );
    QString        language();
    void           setHome();
  };

#endif // WVISUALLIBRARY_H
