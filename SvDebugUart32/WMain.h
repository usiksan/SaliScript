﻿/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Главное окно программы

    Перенес проект в главное окно.
    Имя проекта - это elf файл. Мы храним дату его модификации.
    При модификации файла elf

*/

#ifndef WMAIN_H
#define WMAIN_H

#include "SvConfig.h"
#include "WOscillograph.h"
#include "WDebugTable.h"
#include "SvSymbol.h"

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include <QListWidget>
#include <QtSerialPort/QSerialPort>
#include <QDateTime>


class WMain : public QMainWindow
  {
    Q_OBJECT

    QSerialPort       *mSerial;          //Последовательный порт для связи с устройством
    QSplitter         *mCentral;         //Центральная область - widget в виде разделов, в каждом разделе свое окно
    WOscillograph     *mOscillograph;    //Колонка 1. Осциллограф для наблюдения динамики
    QListWidget       *mSymbolTable;     //Колонка 2. Таблица доступных символов
    QSplitter         *mDebugSplitter;   //Колонка 3. Отладчик
     //Страница с log-выводом
     QListWidget      *mLogList;         //Окно вывода log информации
     //Ячейка с отлаживаемыми переменными
     WDebugTable      *mDebugVar;        //Окно со списком мониторенных переменных

    QString            mElfPath;         //Путь к файлу elf
    QDateTime          mElfTime;         //Время последней модификации elf файла
    QString            mPrjPath;         //Путь к файлу проекта
    QString            mSymPath;         //Путь к файлу с таблицей символов

    SvSymbolMap        mSymbolMap;       //Таблица символов
    QMap<quint32,int>  mRowMap;          //Таблица соответствия адресов ряду в визуальной таблице монитора переменных
    QTimer             mSerialTimer;     //Таймер для обеспечения подключения

    bool               mPause;           //Пауза обмена, во время которой порт отключается

  public:
    WMain( QWidget *parent = nullptr );
    ~WMain();

    void restorePositions();

  public slots:
    void fileOpenProject();
    void fileSaveProject();
    void fileRecentProject();
    void fileProjectOpen( const QString fname );

    void chartsClear();

    void debugClearLog();
    void debugPause();
    void debugResume();

    void settings();

    void helpContens();
    void helpWeb();
    void helpAbout();

    void serialTimer();
    void serialRead();

    void queryVariable( int row, const QString name, int arrayIndex );

    void setVariable( const QString name, int arrayIndex, int value );

  private:
    //Установить заголовок в соотвествии с проектом
    void setupTitle();

    //Обновить таблицу символов
    void updateSymbolTable();

    //Обновить список последних загруженных проектов
    void updateRecentProject();

    // QWidget interface
  protected:
    virtual void closeEvent(QCloseEvent *ev);

  };

#endif // WMAIN_H
