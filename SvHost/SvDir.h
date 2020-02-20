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
#ifndef SVDIR_H
#define SVDIR_H

#include <QString>

/*!
   \brief The SvDir class Представляет собой строку с директорием. Обеспечивает движение
                          по директорию вниз и вверх. Осуществляет декорацию завершающим слэшем.
 */
class SvDir
  {
    QString mCleanedPath; //! Путь без завершающего слэша
  public:
    //!
    //! \brief SvDir Конструктор пути
    //! \param path Путь по умолчанию. Может быть как с завершающим слэшем, так и без
    //!
    SvDir( const QString path = QString() );

    //!
    //! \brief cleanedPath Возвращает путь без завершающего слэша
    //! \return            Путь без завершающего слэша
    //!
    QString cleanedPath() const { return mCleanedPath; }

    //!
    //! \brief slashedPath Возвращает путь с завершающим слэшем
    //! \return            Путь с завершающим слэшем
    //!
    QString slashedPath() const { return mCleanedPath + QString("/"); }

    //!
    //! \brief set  Установить новый путь. Устанавливаемый путь очищается от завершающего слэша
    //!             так как внутреннее хранение - без завершающего слэша
    //! \param path Новый путь
    //!
    void    set( QString path );

    //!
    //! \brief up Поднимается по дереву директориев вверх на одну ступень
    //!
    void    up();

    //!
    //! \brief cd   Опускается по дереву директориев вниз на одну ступень.
    //! \param name Поддиректорий в данном директории, в который нужно зайти
    //!
    void    cd( const QString name );
  };

#endif // SVDIR_H
