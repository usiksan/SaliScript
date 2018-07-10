/*
  Проект "SvStudio"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    22.06.2016 v0.5  в первом приближении налажен обмен по usb
                     исправлены очевидные глюки отладчика (переменные и задачи)
    22.06.2016 v0.6  в компиляторе исправлена ошибка вызова внешних функций
    13.07.2016 v0.7  Сильно поменял SvMirror убрал выполнение операций
                       флагами - оставил загрузку программы и старт-паузу
                     Изменил статус операции, теперь состояние отображается текстом
                     Перелопатил зависимости от изменения SvMirror
                     Устранил ошибку в компиляторе
    16.07.2016 v0.8  создаю зеркала для работы по глобалке и по интернету
    18.07.2016 v0.9  в редактор введена возможность переключать коментарии
    19.07.2016 v0.10 в редактор введена возможность переходить от определения к описанию и наооборот
    20.07.2016 v0.11 в редактор введена помощь по идентификаторам, которая формируется динамически исходя из коментариев
    01.08.2016 v0.12 устранены некоторые ошибки, приводившие к краху
                     добавлена система линковки
                     введены некоторые настройки по умолчанию (IP и порт)
    01.09.2016 v0.13 устранены ошибки компилятора в части массивов
    19.09.2017 v0.14 подключаю систему удаленного управления
                     при создании нового проекта сделал автоматическое добавление расширения
    23.09.2017 v0.15 переделал запрос на получение переменных на запрос для получения полного состояния vpu
    27.09.2017 v0.16 упростил систему компиляции: убрал построитель, представляющий собой массив компиляторов,
                       так как используется единственный компилятор
                     из зеркала убрал тип компилятора и имя узла
                     добавил в зеркало таблицу собранных макроопределений
    27.09.2017 v0.17 убрал из зеркала таблицу собранных макроопределений
    15.05.2018 v0.18 убрал визуализацию данных на основе QChart
    16.05.2018 v0.19 добавил класс визуализации WOscillograph
                       обеспечил систему регистрации и синхронизации
    17.05.2018 v0.20 реализовал отрисовку графиков в WOscillograph
                       добавил скролл-бар для перемещения графиков
    31.05.2018 v0.21 добавил команду генерации определений SvQml... для выделенного фрагмента
    18.06.2018 v0.22 исправил ошибки в генерации определений SvQml...
*/

#ifndef SVCONFIG
#define SVCONFIG

#define SV_VERSION "v0.22"

//Раздел стандартных путей
#define PATH_BASE    "c:/work/SaliLab/SaliDsLibrary/"
#define PATH_HELP    PATH_BASE "help/"
#define PATH_INCLUDE PATH_BASE "include/"
#define PATH_VISUAL  PATH_BASE "visual/"
#define PATH_BOARDS  PATH_BASE "boards/"

#define EXTENSION_PROJECT      ".svproject"
#define EXTENSION_LIB          ".svl"
#define EXTENSION_SCRIPT       ".svs"
#define EXTENSION_BINARY       ".svbin"

#define MAX_RECENT_PROJECT 10 //Количество ранее открытых проектов
#define MAX_RECENT_FILE    10 //Количество ранее открытых файлов

#define DEFAULT_REMOTE_PORT   1971              //Удаленный порт по умолчанию
#define DEFAULT_INTERNET_IP   "62.109.11.83"    //
#define DEFAULT_COMPILER      "TVpu v3"
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

