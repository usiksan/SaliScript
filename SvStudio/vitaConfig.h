/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Конфигурация для приложения SgVita
  История
    15.04.2016 v0.1 создан
    17.04.2016 v0.2 каркас заработал, делаю компилятор
    03.06.2016 v0.3 добавил зеркала, vpu5 придумал мультизеркала
    04.06.2016 v0.4 убирал избыточность кода (оптимизация SgItem)
                    начал добавлять объекты из vpu4
    05.06.2016 v0.5 добавил возможность перехода к другому sml (goto)
                    добавил элемент SgCircle
    10.06.2016 v0.6 добавил встроенные функции dialog и endDialog
                    ввел поддержку Item
    13.06.2016 v0.7 добавил строку редактирования и редактирование значений
                    SgEditLine SgEditValue
    16.07.2016 v0.8 обновленная SvStudio v0.7 - система SvMirror (добавлен локальный сервер, интернет-клиент)
*/

#ifndef VITACONFIG
#define VITACONFIG

#define VERSION_MAJOR  0
#define VERSION_MINOR  8

#define BASE_SML "c:/work/saliLab/SvStudio/sml/main.sml"
//#define BASE_SML QCoreApplication::applicationDirPath() + QString("/sml/main.sml")


#endif // VITACONFIG

