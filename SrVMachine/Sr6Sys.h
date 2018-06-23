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
    Стандартные внешние функции-драйверы, обработка которых
    реализована в классе SrVMachine
*/
#ifndef SR6SYS_H
#define SR6SYS_H

#define VPU_VERSION6      6

#define VPS_VERSION       1  //Получить версию VPU
#define VPS_CREATETASK    2  //Создать новую задачу (активировать следующий VPU)
#define VPS_THROW_CODE    3  //Получить код исключения

#define VPS_LOG           10 //Записать log - файл
#define VPS_LOG_OPEN      11 //Открыть log-файл (организовать в виде стека)
#define VPS_LOG_CLOSE     12 //Закрыть log-файл

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
int vpuTickCount _at_ 1;

import VPS_VERSION      int  srVpuVersion();
//Создание нового виртуального процессора, который будет работать параллельно
import VPS_CREATETASK   int  srCreateTask( void (*fun)(), int stackSize );
import VPS_THROW_CODE   int  srThrowCode();

import VPS_LOG          void srLog( cstring title, int p1, int p2, int p3 );
import VPS_LOG_OPEN     void srLogOpen( cstring fname );
import VPS_LOG_CLOSE    void srLogClose();

import VPS_ABS          int  srAbs( int val );
import VPS_IMIN         int  srIMin( int v1, int v2 );
import VPS_IMAX         int  srIMax( int v1, int v2 );
import VPS_UMIN         int  srUMin( int v1, int v2 );
import VPS_UMAX         int  srUMax( int v1, int v2 );
import VPS_ILIMIT       int  srILimit( int val, int minVal, int maxVal );
import VPS_FACTOR106    int  Factor106( int val, int factor );
import VPS_FACTOR103    int  Factor103( int val, int factor );
import VPS_DEFACTOR106  int  DeFactor106( int val, int factor );
import VPS_DEFACTOR103  int  DeFactor103( int val, int factor );

import VPS_RND          int  srRandom( int minVal, int maxVal );

void
svWaitTick( int time ) {
  time += vpuTickCount;
  while( (time - vpuTickCount) > 0 )
    Wait();
  }
#endif


#endif // SR6SYS_H
