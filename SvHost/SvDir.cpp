/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "SvHost управление скриптами из компьютера"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    SvDir строка, содержащая директорий
*/
#include "SvDir.h"

SvDir::SvDir(const QString path)
  {
  set( path );
  }



//!
//! \brief set  Установить новый путь. Устанавливаемый путь очищается от завершающего слэша
//!             так как внутреннее хранение - без завершающего слэша
//! \param path Новый путь
//!
void SvDir::set(QString path)
  {
  //Если путь завершается слэшем, то удаляем его
  //в противном случае присваиваем без изменений
  if( path.endsWith( QChar('/')) )
    mCleanedPath = path.left( path.length() - 1 );
  else
    mCleanedPath = path;
  }



//!
//! \brief up Поднимается по дереву директориев вверх на одну ступень
//!
void SvDir::up()
  {
  //Ищем крайний справа слэш
  int slash = mCleanedPath.lastIndexOf( QChar('/') );
  //Если нашли, то отсекаем все, что справа от этого символа и сам символ
  if( slash > 0 )
    mCleanedPath = mCleanedPath.left( slash );
  }



//!
//! \brief cd   Опускается по дереву директориев вниз на одну ступень.
//! \param name Поддиректорий в данном директории, в который нужно зайти
//!
void SvDir::cd(const QString name)
  {
  //Добавляем к пути имя поддиректория
  set( slashedPath() + name );
  }


