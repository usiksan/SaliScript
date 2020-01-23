/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    21.01.2020 v0.1  Начал проект
    22.01.2020 v0.2  Довел до безошибочной компиляции
               v0.3  Добавил колонку со списком доступных переменных
               v0.4  Добавил поддержку прежних проектов
    23.01.2020 v0.5  Добавляю диалог настройки порта отладки
*/

#ifndef SVCONFIG
#define SVCONFIG

#define SV_VERSION "v0.5"

#define PATH_HELP              ""
#define EXTENSION_PROJECT      ".svdebug"

#define MAX_RECENT_PROJECT 10 //Количество ранее открытых проектов
#define MAX_RECENT_FILE    10 //Количество ранее открытых файлов

#define DEFAULT_REMOTE_PORT   1971              //Удаленный порт по умолчанию
#define DEFAULT_INTERNET_IP   "62.109.11.83"    //
#define DEFAULT_COMPILER      "TVpu v3"
#define DEFAULT_HOME_WEB      "www.salilab.ru"

//Разделы конфигурации
#define CFG_LANGUAGE          "language"
#define CFG_PROJECTS          "projects"
#define CFG_WMAIN_MAX         "window main maximize"
#define CFG_LIBRARY_LIST      "library list"
#define CFG_VISUAL_SPLITTER   "visual splitter"
#define CFG_TEXT_SPLITTER     "text splitter"
#define CFG_PROJECT_SPLITTER  "project splitter"
#define CFG_DEBUG_SPLITTER    "debug splitter"
#define CFG_BRIDGE_IP         "bridge ip"
#define CFG_BRIDGE_PORT       "bridge port"
#define CFG_BRIDGE_ID         "bridge id"
#define CFG_BRIDGE_PASSW      "bridge passw"

#define CFG_DEBUG_PORT        "debug port"
#define CFG_DEBUG_BAUDRATE    "debug baudrate"

#define CFG_OSCIL_ROW_MAX     "osc row max"  //Максимальное количество выборок в осциллографе
#define CFG_OSCIL_MODE        "osc mode"     //Режим работы
#define CFG_OSCIL_SYNC        "osc sync"     //Режим синхронизации
#define CFG_OSCIL_LEVEL       "osc level"    //Уровень синхронизации
#define CFG_OSCIL_INTERVAL    "osc interval" //Интервал опроса данных

#define KEY_COLOR_BACK        "color back"
#define KEY_COLOR_AXIZ        "color axiz"
#define KEY_COLOR_GRID        "color grid"

#define CFG_AUTOINDENT_SPACE_COUNT "autoIndentSpaceCount"
#define CFG_AUTOCOMPLETE_PARENTHESIS "autoCompleteParenthesis"


#endif // SVCONFIG

