/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Список команд и конструктор меню
*/

#ifndef MTCOMMAND
#define MTCOMMAND

#include "SvConfig.h"
#include <QAction>
#include <QMenuBar>

extern QAction
//Главное меню
*maFile,   //Меню файл
*maFileOpenProject,
*maFileRecentProject,
*maFileSaveProject,
*maFileExit,

//Меню Отладка
*maDebug,
*maDebugClearLog, //Очистить лог
*maDebugPause,    //Приостановить сканирование и освободить порт
*maDebugResume,   //Снова подключиться к порту и возобновить сканирование

//Меню Настройки
*maSettings,

//Меню Справка
*maHelp,
*maHelpContens,
*maHelpWeb,
*maHelpAbout,

*maLast;

typedef QAction *QActionPtr;

extern QActionPtr maRecentProject[MAX_RECENT_PROJECT];

class WMain;
void CommandCreateActions( WMain *p, QMenuBar *bar );

#endif // MTCOMMAND

