/*
   Проект
   */

#ifndef SVDEBUGUART32_H
#define SVDEBUGUART32_H

#define SV_CB_INFO_GET   'I' //Запросить информацию о версии ПО
#define SV_CB_INFO       'i' //Информация о версии ПО
                             // i8[32] - тип контроллера

#define SV_CB_MEMORY_GET 'R' //Запросить содержимое ячейки памяти. Выдается содержимое ячейки памяти
                             // i32     - адрес ячейки памяти

#define SV_CB_MEMORY     'M' //Содержимое ячейки памяти
                             // i32 - адрес ячейки памяти
                             // i32 - содержимое ячейки памяти


#endif // SVDEBUGUART32_H
