/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Пико-компилятор"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Описание команд взаимодействия через сеть
*/
#ifndef SVNETCOMMANDSET_H
#define SVNETCOMMANDSET_H

#include <QString>
#include <QByteArray>
#include <QDataStream>

//Подключение к мосту
#define SVC_BRIDGE_LINK    1 //Запрос на подключение к мосту
                             //data: SvNetBridgeLink

#define SVC_BRIDGE_STATUS  2 //Состояние моста
                             //data: SvNetBridgeStatus


//Подключение к объекту
#define SVC_INFO_REQUEST   3 //Запрос информации об объекте
                             //data: SvNetInfoRequest

#define SVC_INFO           4 //Информация об объекте
                             //data: SvNetInfo


//Обмен файлами
#define SVC_DIR_REQUEST   20 //Запрос информации о файлах директория
                            //data: SvNetDirRequest

#define SVC_DIR_LIST      21 //Список файлов директория
                             //data: SvNetDirList

#define SVC_FILE_REQUEST  22 //Запрос на загрузку файла
                             //data: SvNetFileRequest

#define SVC_FILE          23 //Передача файла
                             //data: SvNetFile


//Удаленный просмотр и управление
#define SVC_SCREEN        40 //Содержимое экрана
                             //data: SvNetScreen

#define SVC_MOUSE_PATH    41 //Путь мыши
                             //data: SvNetMousePath

#define SVC_KEYBOARD      42 //Очередь нажатий клавиш
                             //data: SvNetKeyboard


//Установка новой программы
#define SVC_FLASH_RUN     60 //Установка новой программы
                             //data: SvNetFlashRun

#define SVC_VARIABLES     61 //Список переменных


#define SVC_TASKS         62 //Список задач


#define SVC_RESTART       63

#define SVC_
struct SvNetInfoRequest {
    QString mName;
  };

struct SvNetFlashRun {

  };

#endif // SVNETCOMMANDSET_H
