/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
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
//Переключение режимов работы
*maModeIntro,      //Режим введения
*maModeBoard,      //Режим настройки платы
*maModeText,       //Режим текстового проектирования
*maModeHelp,       //Режим помощи
*maWOscillograph,  //Окно с осциллографом

//Главное меню
*maFile,   //Меню файл
*maFileNew,
*maFileNewProject,
*maFileOpen,
*maFileOpenOver,
*maFileRecentProject,
*maFileRecentFile,
*maFileReceivProject,
*maFileSave,
*maFileSaveAll,
*maFileSaveAs,
*maFileSaveProject,
*maFileClose,
*maFileCloseAll,
*maFilePrint,
*maFileExit,

//Меню правка
*maEdit,
*maEditUndo,
*maEditRedo,
*maEditCut,
*maEditPaste,
*maEditCopy,
*maEditSelectAll,
*maEditSearch,
*maEditReplace,
*maEditAutoIndent,
*maEditEditorSettings,
*maEditSvQmlGenerate,

//Меню Визуальное
*maVisual,
*maVisualInsert,

//Меню сборка
*maBuild,
*maBuildBuild,    //Компиляция проекта
*maBuildAndRun,   //Компиляция и запуск
*maBuildAndPause, //Компиляция и пауза
*maBuildAndLink,  //Компиляция, линковка, если программа одинаковая, прошивка и перезапуск - если разная

//Подменю выбора канала
*maChannelSimulator, //Симулятор
*maChannelUsb,       //В аппаратуре
*maChannelRemote,    //Удаленная отладка
*maChannelBridge,    //Remote debug over internet bridge

//Меню исполнения, отладки и симуляции
*maDebug,
*maDebugChannel,  //Канал с исполняющей системой
*maDebugLoad,     //Загрузить программу в исполняющую систему
*maDebugLink,     //Подключиться к исполняющейся программе
*maDebugFlash,    //Прошить программу в исполняющую систему

*maDebugRun,      //Компиляция и запуск исполнения
*maDebugPause,    //Остановить исполнение
*maDebugStep,     //Выполнить один шаг текущего виртуального процессора
*maDebugTrace,    //Выполнить один шаг с заходом в функцию
*maDebugReset,    //Начать исполнение заново
*maDebugAddWatch, //Добавить элемент к наблюдению
*maDebugRunAll,   //Запустить все задачи
*maDebugPauseAll, //Приостановить все задачи
*maDebugClearLog, //Очистить лог

*maErrorPrev,     //Перейти к предыдущей ошибке
*maErrorNext,     //Перрейти к следующей ошибке

//Меню Справка
*maHelp,
*maHelpContext,
*maHelpContens,
*maHelpWeb,
*maHelpAbout,

*maHelpCxtHome,     //К началу контекстной помощи
*maHelpCxtBackward, //К предыдущей странице контекстной помощи
*maHelpCxtForward,  //К следующей странице контекстной помощи

//Команды библиотекаря
*maLibHome,
*maLibBack,
*maLibForward,
*maLibApply,



*maLast;

typedef QAction *QActionPtr;

extern QActionPtr maRecentProject[MAX_RECENT_PROJECT];
extern QActionPtr maRecentFile[MAX_RECENT_FILE];

class WMain;
void CommandCreateActions( WMain *p, QMenuBar *bar );

#endif // MTCOMMAND

