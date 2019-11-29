#ifndef SVVMINTERFACE_H
#define SVVMINTERFACE_H


//Справочные
#define SVU_CMD_VERSION_GET    1 //Получить версию ПО
                               //данные - нет
#define SVU_CMD_VERSION        2 //Controller version and type
                               //ответ [0-3]   номер версии
                               //      [4-7]   размер памяти программ
                               //      [8-11]  размер памяти данных
                               //      [12]    тип контроллера (процессор и количество слотов), из этой информации определяются возможности контроллера
                               //      [13]    максимальное количество VPU
                               //      [14]    фактическое количество активных vpu
                               //      [15]
                               //      [16-23] сигнатура установленной программы
                               //      [24-27] размер прошитой программы
                               //      [28-31] количество глобальных переменных
                               //      [31]    SVU_ANS_OK

#define SVU_CMD_STATE_GET      3
#define SVU_CMD_STATE          4


#define SVU_CMD_FLASH_ERASE    5 //Стереть память программ
                                 //Данных нет
#define SVU_CMD_FLASH_ERASE_OK 6
#define SVU_CMD_FLASH_ERASE_FAIL 7

#define SVU_CMD_FLASH_READ       8 //Прочитать блок данных из Flash
                               //данные [0-3]   (4 байта) количество читаемых данных
                               //       [4-7]   (4 байта) адрес начала чтения относительно начала программ
#define SVU_CMD_FLASH_BLOCK      9
                               //ответ блок данных памяти программ

#define SVU_CMD_FLASH_WRITE     10 //Записать блок данных во Flash
                               //данные [0-3]   (4 байта) адрес блока (относительно начала программы)
                               //       [4-59]  (56 байт) блок данных для записи
#define SVU_CMD_FLASH_WRITE_OK  11
#define SVU_CMD_FLASH_WRITE_BUSY 12
                               //ответ SVU_ANS_OK или SVU_ANS_BUSY



//Работа с памятью данных
#define SVU_CMD_VARS_READ       13 //Прочитать значения переменных
                               //данные [0-3]   (4 байта) количество читаемых ячеек
                               //       [4-7]   (4 байта) индекс начальной ячейки для чтения
#define SVU_CMD_VARS_BLOCK      14
                               //ответ блок данных с переменными

#define SVU_CMD_VARS_WRITE      15 //Записать значения в переменные
                               //данные [0] - кол-во пар адрес-значение (максимум 10)
                               //       [1-2] адрес
                               //       [3-6] значение
                               //       ...
#define SVU_CMD_VARS_WRITE_OK   16
#define SVU_CMD_VARS_WRITE_BUSY 17
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY




#define SVU_CMD_VPU_STATE_GET   18 //Количество и состояние VPU
                               //данные - нет
#define SVU_CMD_VPU_STATE       19 //SvVpuState structure
                               //ответ [0] - количество VPU
                               //      [1-62] - состояния VPU (0-halt, 1-running, 2-paused, 3-vpu not used)
                               //      [63] - SVU_ANS_OK


#define SVU_CMD_VPU_RESTART     20 //Перезапустить текущую программу с заданными флагами отладки
                               //данные [0] запускать без отладки (1-без отладки, 0-с отладкой)
#define SVU_CMD_VPU_RESTART_OK  21
#define SVU_CMD_VPU_RESTART_BUSY 22
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY

#define SVU_CMD_DEBUG_RUN   34 //Запустить на исполнение заданный VPU
                               //данные [0] номер VPU
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY

#define SVU_CMD_DEBUG_PAUSE 35 //Остановить заданный VPU
                               //данные [0] номер VPU
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY

#define SVU_CMD_DEBUG_TRACE 36 //Трассировка (шаг со входом в пп) для заданного VPU
                               //данные [0] номер VPU
                               //       [1-4] нижняя граница
                               //       [5-8] верхняя граница
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY

#define SVU_CMD_DEBUG_STEP  37 //Шаг для заданного VPU
                               //данные [0] номер VPU
                               //       [1-4] нижняя граница
                               //       [5-8] верхняя граница
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY

#define SVU_CMD_DEBUG_UNTIL 38 //Выполнять до условия для заданного VPU
                               //данные [0] номер VPU
                               //       [1-4] нижняя граница
                               //       [5-8] верхняя граница
                               //ответ PLC_ANS_OK или PLC_ANS_BUSY


//Ответ-подтверждение
#define SVU_ANS_OK        0x7e
//Ответ-ошибка операции
#define SVU_ANS_ERROR     0x11
//Ответ-устройство занято, последняя операция должна быть повторена
#define SVU_ANS_BUSY      0xc2

#endif // SVVMINTERFACE_H
