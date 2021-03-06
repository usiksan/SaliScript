﻿/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
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
    24.02.2020 v0.7  Исправил ошибку в записи имени программы в SvProgramm
                     Добавил возможность задания имени локального контроллера
               v0.8  Исправил процесс переподключения при повторном запуске клиента
               v0.9  Добавил передачу логов по сети
    27.02.2020 v0.10 Исправил позиционирование в отладчике
                     Добавил запись в программу перечень исходных файлов для правильного позиционирования отладчика
                     Исправил шаг-трасса при отладке
    29.02.2020 v0.11 В виртуальную машину добавил механизм извещений для взаимодействия с внешними
                     программами, а также для вызова удаленных функций
               v0.12 Начал делать зеркало для микроконтроллера через usb-com
    03.03.2020 v0.13 Удалил зеркало для libusb. Подключил к проекту библиотеку serialport
    27.03.2020 v0.14 Сделал подключение к контролллеру через SvMirrorCom
                     Добавил прошивку программы
                     Добавил чтение глобальных переменных
               v0.15 Пробую добавить отладку
               v0.16 Отладку добавил. Добавляю запись переменных
    29.03.2020 v0.17 Добавил описание в SvMirrorCom, а также решил проблему с отображением
                     серийного номера контроллера.
                     Пробую избавиться от проблем с подключением
               v0.18 Переделал SvMirrorCom на систему запрос-ответ-... Здесь мы после подключения
                     отправляем запрос, а следующий запрос отправляем только после получения ответа.
                     Если ответ не приходит в течение определенного интервала, то переподключаемся.
               v0.19 Обновил виртуальную машину. Устранение ошибки вызова встроенной функции
               v0.20 Добавил поддержку лога из контроллера
    08.04.2020 v0.21 Ошибка: не применялись изменения настроек проекта в зеркале
    14.04.2020 v0.22 Ошибка: чтение в разных потоках SvMirrorExtern::mVpuState приводит к зависанию
                     из-за shared-object. Решение: делаю полную копию и отдаю ее.
    16.04.2020 v0.23 Дополнительный сервис. Теперь менджер зеркал нужно стартовать явно
    25.04.2020 v0.24 В список символов программы добавил глобальные функции
*/

#ifndef SVCONFIG
#define SVCONFIG

#define SV_VERSION "v0.24"

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

