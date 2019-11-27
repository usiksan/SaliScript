/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef CMODEHELP_H
#define CMODEHELP_H

#include "SvConfig.h"
#include <QWidget>
#include <QTextBrowser>

class WCModeHelp : public QTextBrowser {
    Q_OBJECT
  public:
    explicit WCModeHelp(QWidget *parent = nullptr);
    ~WCModeHelp();

    //Текущая библиотека
    static QString mCurrentLib;

    //Преобразование названия страницы в фактический URL в соответствии с выбранным языком
    static QUrl    pageConvert( const QString &page , const QString &fragment);

    //Вернуть страницу помощи с ошибкой ссылки
    static QUrl    pageError();

    //Выдать домашнюю страницу относительно текущей библиотеки
    static QUrl    pageLibraryHead();

    //Выдать глобальную домашнюю страницу
    static QUrl    pageHead();
  signals:

  public slots:
    void contens();
  };

#endif // CMODEHELP_H
