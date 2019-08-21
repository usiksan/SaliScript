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
#ifndef SV6SYS_H
#define SV6SYS_H

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

import VPS_VERSION      int  svVpuVersion();
//Создание нового виртуального процессора, который будет работать параллельно
import VPS_CREATETASK   int  svCreateTask( void (*fun)(), int stackSize );
import VPS_THROW_CODE   int  svThrowCode();

import VPS_LOG          void svLog( cstring title, int p1, int p2, int p3 );
import VPS_LOG_OPEN     void svLogOpen( cstring fname );
import VPS_LOG_CLOSE    void svLogClose();

import VPS_ABS          int  svAbs( int val );
import VPS_IMIN         int  svIMin( int v1, int v2 );
import VPS_IMAX         int  svIMax( int v1, int v2 );
import VPS_UMIN         int  svUMin( int v1, int v2 );
import VPS_UMAX         int  svUMax( int v1, int v2 );
import VPS_ILIMIT       int  svILimit( int val, int minVal, int maxVal );
import VPS_FACTOR106    int  svFactor106( int val, int factor );
import VPS_FACTOR103    int  svFactor103( int val, int factor );
import VPS_DEFACTOR106  int  svDeFactor106( int val, int factor );
import VPS_DEFACTOR103  int  svDeFactor103( int val, int factor );

import VPS_RND          int  svRandom( int minVal, int maxVal );

void
svWaitTick( int time ) {
  time += vpuTickCount;
  while( (time - vpuTickCount) > 0 )
    Wait();
  }
#endif


#endif // SV6SYS_H
