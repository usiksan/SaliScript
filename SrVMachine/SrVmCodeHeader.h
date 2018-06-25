/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Виртуальная машина"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Поля заголовка программы виртуальной машины.

    Так как программа байт-кода представляет собой последовательность байтов,
    то поля заголовка программы представлены ссылками начала соответствующих полей
*/
#ifndef SRVMCODEHEADER_H
#define SRVMCODEHEADER_H


/*!
  \defgroup srVmProgrammHeader
  \brief Структура размещения данных в коде

  Так как программа байт-кода представляет собой последовательность байтов,
  то поля заголовка программы представлены ссылками начала соответствующих полей
*/
#define SRVMH_HASH              0 //! Хэш-значение кода 4 байта
#define SRVMH_SIGNATURE         4 //! Текстовое поле сигнатуры длиной 20 байт
#define SRVMH_SIGNATURE_LENGHT 20 //! Длина поля сигнатуры
#define SRVMH_INIT_TABLE       28 //! Адрес начала таблицы инициализации 4 байт
                                  //! -Каждая запись таблицы адрес 3 байта : значение 4 байта
                                  //! -Последняя запись - нуль
#define SRVMH_ENTER_POINT      32 //! Адрес стартовой функции 4 байта
#define SRVMH_STACK_SIZE0      36 //! Размер стека стартовой задачи 4 байта
#define SRVMH_VM_PROG_SIZE     40 //! Размер памяти программ без учета таблиц, т.е. чистая программа 4 байта
#define SRVMH_GLOBAL_COUNT     44 //! Количество глобальных переменных 4 байта
#define SRVMH_CONST_TABLE      48 //! Адрес таблицы константных блоков 4 байта (cblock и cstring)
                                  //! Каждая запись содержит 4 байта с адресом начала блока
#define SRVMH_PROG_SIZE        52 //! Размер всей программы 4 байта

#define SRVMH_HEADER_SIZE      56 //! Размер заголовка



#endif // SRVMCODEHEADER_H
