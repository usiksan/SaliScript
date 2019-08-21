/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef DSLIBCATEGORY_H
#define DSLIBCATEGORY_H

#include <QDir>
#include <QString>
#include <QList>

struct DsLibCategory;
typedef DsLibCategory *DsLibCategoryPtr;
typedef QList<DsLibCategoryPtr> DsLibCategoryPtrList;

struct DsLibCategory
  {
    QString            mTitle;       //Наименование категории на языке пользователя
    QString            mIcon;        //Иконка категории
    QString            mDescription; //Описание категории на языке пользователя
    QStringList        mSubCategory;
    //LibCategoryPtrList mSubCategory; //Список подкатегорий
    QStringList        mObjectList;  //Список объектов данной категории

    DsLibCategory( QDir libDir, const QString categoryName, const QString noDescription );
    ~DsLibCategory();
  };

typedef QMap<QString,DsLibCategoryPtr> DsLibCategoryMap;


#endif // DSLIBCATEGORY_H
