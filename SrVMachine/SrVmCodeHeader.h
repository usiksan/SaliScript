/*
  Project     "Script language reduced c++ (rc++)"
  SubProject  "Virtual machine"
  Author
    Alexander Sibilev
  Internet
    www.rc.saliLab.ru - project home site
    www.saliLab.ru
    www.saliLab.com

  Description
    Virtual machine programm header
    At begining each byte-code programm there is a header with service information
    In this file we descript all field of this header
*/
#ifndef SRVMCODEHEADER_H
#define SRVMCODEHEADER_H

//Header service information structure [Структура размещения данных в коде]
#define SRVMH_HASH              0 //Hash value of programm 4 bytes [Хэш-значение кода 4 байта]
#define SRVMH_SIGNATURE         4 //Textual programm name 20 bytes (20 chars) [Текстовое поле длиной 20 байт]
#define SRVMH_SIGNATURE_LENGHT 20 //Textual programm name lenght [Длина поля сигнатуры]
#define SRVMH_INIT_TABLE       24 //Addr of init table 4 bytes [Адрес начала таблицы инициализации 4 байт]
                                  //Each record of table is addr 3 bytes : value 4 bytes [Каждая запись таблицы адрес 3 байта : значение 4 байта]
                                  //Last record is null [Последняя запись - нуль]
#define SRVMH_ENTER_POINT      28 //Start function address 4 bytes [Адрес стартовой функции 4 байта]
#define SRVMH_STACK_SIZE0      32 //Stack size of start task 4 bytes [Размер стека стартовой задачи 4 байта]
#define SRVMH_VM_PROG_SIZE     36 //Programm memory size 4 bytes [Размер памяти программ 4 байта]
#define SRVMH_GLOBAL_COUNT     40 //Global variables count 4 bytes [Количество глобальных переменных 4 байта]
#define SRVMH_CONST_TABLE      44 //Const block (aka cstring and cblock) table address 4 bytes [Адрес таблицы константных блоков 4 байта]
                                  //Each record contains 4 bytes of const block address
#define SRVMH_PROG_SIZE        48 //Full programm size 4 bytes [Размер всей программы 4 байта]

#define SRVMH_HEADER_SIZE      52 //Header size [Размер заголовка]



#endif // SRVMCODEHEADER_H
