/*
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

     bool              mVarChangeLock;   //Блокировка от изменения ячейки внутренними средствами

     QString           mElfPath;         //Путь к файлу elf
     QDateTime         mElfTime;         //Время последней модификации elf файла
     QString           mPrjPath;         //Путь к файлу проекта
     QString           mSymPath;         //Путь к файлу с таблицей символов

     SvSymbolMap       mSymbolMap;
     QMap<int,quint32> mRowMap;

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

    void helpContens();
    void helpWeb();
    void helpAbout();


  private:
    //Установить заголовок в соотвествии с проектом
    void setupTitle();

    //Обновить таблицу символов
    void updateSymbolTable();

    // QWidget interface
  protected:
    virtual void closeEvent(QCloseEvent *ev);

  };

#endif // WMAIN_H
