/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "SvStudio"
    IDE скриптового языка
  Автор
    Сибилев А.С.
  Описание
    2019       v0.1  Перешел на виртуальную машину V6
    21.01.2020 v0.2  Исправил подсветку строк
    20.02.2020 v0.3  Перековырял SvMirror для согласованности с удаленной отладкой через Ethernet
                     Сделал зеркало-основу для удаленной отладки
                     Сделал зеркало для отладки через локальную сеть
                     Сделал обработчик для отладки через локальную сеть
                     Вернул возможность пошагового исполнения
                     Скорректировал позиционирование отлаживаемой строки
               v0.4  Модифицировал проект для хранения настроек удаленного доступа
               v0.5  Добавил возможность удаленного доступа по локальной сети
    21.02.2020 v0.6  Произвожу чистку проекта от шелухи
*/

#ifndef SVCONFIG
#define SVCONFIG

#define SV_VERSION "v0.6"

#define PATH_HELP              ""
#define EXTENSION_PROJECT      ".svproject"
#define EXTENSION_SCRIPT       ".svs"
#define EXTENSION_BINARY       ".svbin"

#define MAX_RECENT_PROJECT 10 //Количество ранее открытых проектов
#define MAX_RECENT_FILE    10 //Количество ранее открытых файлов

#define DEFAULT_REMOTE_PORT   1971              //Удаленный порт по умолчанию
#define DEFAULT_INTERNET_IP   "62.109.11.83"    //
#define DEFAULT_HOME_WEB      "www.salilab.ru"

//Разделы конфигурации
#define CFG_LANGUAGE          "language"
#define CFG_PROJECTS          "projects"
#define CFG_FILES             "files"
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

