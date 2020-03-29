/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "Виртуальная машина"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Стандартные внешние функции-драйверы, обработка которых
    реализована в классе SrVMachine
*/
#ifndef SV7SYS_H
#define SV7SYS_H

#define VPU_VERSION7      7

#define VPS_VERSION       1  //Получить версию VPU
#define VPS_CREATETASK    2  //Создать новую задачу (активировать следующий VPU)
#define VPS_THROW_CODE    3  //Получить код исключения
#define VPS_TICKET00      4  //Извещение для синхронизации с внешними программами, а также для
                             //организации вызова удаленных функций
#define VPS_TICKET10      5
#define VPS_TICKET20      6
#define VPS_TICKET40      7
#define VPS_TICKET80      8
#define VPS_TICKET01      9
#define VPS_TICKET11      10
#define VPS_TICKET22      11
#define VPS_TICKET44      12
#define VPS_TICKET26      13


#define VPS_LOG           14 //Записать log - файл
#define VPS_LOG_OPEN      15 //Открыть log-файл (организовать в виде стека)
#define VPS_LOG_CLOSE     16 //Закрыть log-файл

#define VPS_WAIT_TIME_OUT 17 //Ожидать пока время не превысит заданное
#define VPS_IS_TIME_OUT   18 //Проверить, истекло ли заданное время

//Математика
#define VPS_ABS           20 //Абсолютное значение числа
#define VPS_IMIN          21 //Получение минимального из двух значений
#define VPS_IMAX          22 //Получение максимального из двух значений
#define VPS_UMIN          23 //Получение минимального из двух значений для беззнакового числа
#define VPS_UMAX          24 //Получение максимального из двух значений для беззнакового числа
#define VPS_ILIMIT        25 //Ограничение целого числа
#define VPS_FACTOR106     26 //Преобразование (умножение) с использованием арифметики двойной точности. Значение в 0.001ед. Фактор представлен в 0.001 ед.
#define VPS_FACTOR103     27 //Преобразование (умножение) с использованием арифметики двойной точности. Значение в 1ед. Фактор представлен в 0.001 ед.
#define VPS_DEFACTOR106   28 //Преобразование (деление) с использованием арифметики двойной точности. Значение в 0.001ед. Фактор представлен в 0.001 ед.
#define VPS_DEFACTOR103   29 //Преобразование (деление) с использованием арифметики двойной точности. Значение в 1ед. Фактор представлен в 0.001 ед.

//Прочие функции
#define VPS_RND           40 //Получить случайное число в заданном диапазоне

#define VPS_EEPROM_WRITE  50 //Записать значение в EEPROM (для стационарной машины записывается в конфиг)
#define VPS_EEPROM_READ   51 //Прочитать значение из EEPROM


#ifdef VPU_COMPILER
//Обязательная переменная отражающая счетчик милисекунд
int svTickCount _at_ 1;

import VPS_VERSION       int  svVpuVersion();
//Создание нового виртуального процессора, который будет работать параллельно
import VPS_CREATETASK    int  svCreateTask( void (*fun)(), int stackSize );
import VPS_THROW_CODE    int  svThrowCode();
import VPS_TICKET00      void svTicket0( int ticketId );
import VPS_TICKET10      void svTicket1( int ticketId, int p0 );
import VPS_TICKET20      void svTicket2( int ticketId, int p0, int p1 );
import VPS_TICKET40      void svTicket4( int ticketId, int p0, int p1, int p2, int p3 );
import VPS_TICKET80      void svTicket8( int ticketId, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7 );
import VPS_TICKET01      void svTicket0r1( int ticketId, int *r0 );
import VPS_TICKET11      void svTicket1r1( int ticketId, int p0, int *r0 );
import VPS_TICKET22      void svTicket2r2( int ticketId, int p0, int p1, int *r0, int *r1 );
import VPS_TICKET44      void svTicket4r4( int ticketId, int p0, int p1, int p2, int p3, int *r0, int *r1, int *r2, int *r3 );
import VPS_TICKET26      void svTicket2r6( int ticketId, int p0, int p1, int *r0, int *r1, int *r2, int *r3, int *r4, int *r5 );


import VPS_LOG           void svLog( cstring title, int p1, int p2, int p3 );
import VPS_LOG_OPEN      void svLogOpen( cstring fname );
import VPS_LOG_CLOSE     void svLogClose();

import VPS_WAIT_TIME_OUT void svWaitTimeOut( int time );
import VPS_IS_TIME_OUT   int  svIsTimeOut( int time );

import VPS_ABS           int  svAbs( int val );
import VPS_IMIN          int  svIMin( int v1, int v2 );
import VPS_IMAX          int  svIMax( int v1, int v2 );
import VPS_UMIN          int  svUMin( int v1, int v2 );
import VPS_UMAX          int  svUMax( int v1, int v2 );
import VPS_ILIMIT        int  svILimit( int val, int minVal, int maxVal );
import VPS_FACTOR106     int  svFactor106( int val, int factor );
import VPS_FACTOR103     int  svFactor103( int val, int factor );
import VPS_DEFACTOR106   int  svDeFactor106( int val, int factor );
import VPS_DEFACTOR103   int  svDeFactor103( int val, int factor );

import VPS_RND           int  svRandom( int minVal, int maxVal );

void
svWaitTick( int time ) {
  svWaitTimeOut( time + svTickCount );
  }

int svIsTimeOut( int time ) {
  return (time - svTickCount) <= 0;
  }
#endif


#endif // SV6SYS_H
