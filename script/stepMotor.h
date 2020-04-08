/*
Проект "Виртуальный процессор VPU"
Автор
  Сибилев А.С.
Описание
  Скриптовая инфраструктура.

  Системные вызовы.

*/
#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <spi.h>

//Тестирование модуля шагового двигателя
#define SPIN_NOP          0x00
#define SPIN_SET_PARAM    0x00
#define SPIN_GET_PARAM    0x20
#define SPIN_RUN          0x50
#define SPIN_STEP_CLOCK   0x58
#define SPIN_MOVE         0x40
#define SPIN_GO_TO        0x60
#define SPIN_GO_TO_DIR    0x68
#define SPIN_GO_UNTIL     0x82
#define SPIN_RELEASE_SW   0x92
#define SPIN_GO_HOME      0x70
#define SPIN_GO_MARK      0x78
#define SPIN_RESET_POS    0xD8
#define SPIN_RESET_DEVICE 0xC0
#define SPIN_SOFT_STOP    0xB0
#define SPIN_HARD_STOP    0xB8
#define SPIN_SOFT_HIZ     0xA0
#define SPIN_HARD_HIZ     0xA8
#define SPIN_GET_STATUS   0xD0


#define SPIN_ABS_POS      0x01
#define SPIN_EL_POS       0x02
#define SPIN_MARK         0x03
#define SPIN_SPEED        0x04
#define SPIN_ACC          0x05
#define SPIN_DEC          0x06
#define SPIN_MAX_SPEED    0x07
#define SPIN_MIN_SPEED    0x08
#define SPIN_FS_SPD       0x15
#define SPIN_KVAL_HOLD    0x09
#define SPIN_KVAL_RUN     0x0A
#define SPIN_KVAL_ACC     0x0B
#define SPIN_KVAL_DEC     0x0C
#define SPIN_INT_SPD      0x0D
#define SPIN_ST_SLP       0x0E
#define SPIN_FN_SLP_ACC   0x0F
#define SPIN_FN_SLP_DEC   0x10
#define SPIN_K_THERM      0x11
#define SPIN_ADC_OUT      0x12
#define SPIN_OCD_TH       0x13
#define SPIN_STALL_TH     0x14
#define SPIN_STEP_MODE    0x16
#define SPIN_ALARM_EN     0x17
#define SPIN_CONFIG       0x18
#define SPIN_STATUS       0x19

//Получить состояние привода
int
StepmState( int module ) {
  return SpiTransfer( module, SPIN_GET_STATUS, 2, 0 );
  }
  
  
  
//Определить, дошел ли механизм до нужной позиции
int
StepmIsMoving( int module ) {
  return ~SpiTransfer( module, SPIN_GET_STATUS, 2, 0 ) & 2;
  }
  
  

//Сбросить модуль
void
StepmReset( int module ) {
  SpiTransfer( module, SPIN_RESET_DEVICE, 0, 0 );
  }

//Установить позицию
void
StepmSetPos( int module, int pos ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_ABS_POS, 3, pos );
  }

int
StepmGetPos( int module ) {
  module = SpiTransfer( module, SPIN_GET_PARAM|SPIN_ABS_POS, 3, 0 );
  //Расширяем знак
  if( module & 0x200000 )
    return module | 0xffe00000;
  return module;
  }
  
  
//Перейти к заданной позиции с заданием направления движения
void
StepmMoveAbsToDir( int module, int forward, int pos ) {
  if( forward ) forward = 1 | SPIN_GO_TO_DIR;
  else forward = SPIN_GO_TO_DIR;
  SpiTransfer( module, forward, 3, pos );
  }


  
//Перейти к заданной позиции с автоматическим определением направления движения
void
StepmMoveAbsTo( int module, int pos ) {
  SpiTransfer( module, SPIN_GO_TO, 3, pos );
  }


  
//Запустить вращение
void
StepmRun( int module, int forward, int speed ) {
  if( forward ) forward = 1 | SPIN_RUN;
  else forward = SPIN_RUN;
  SpiTransfer( module, forward, 3, speed );
  }

//Остановить
void
StepmSoftStop( int module ) {
  SpiTransfer( module, SPIN_SOFT_STOP, 0, 0 );
  }

void
StepmHardStop( int module ) {
  SpiTransfer( module, SPIN_HARD_STOP, 0, 0 );
  }

//Установить режим микрошагов
void
StepmSetStepMode( int module, int mode ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_STEP_MODE, 1, mode );
  }

//Установить конфигурацию
void
StepmSetConfig( int module, int cfg ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_CONFIG, 2, cfg );
  }

//Ток удержания
void
StepmSetKHold( int module, int k ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_KVAL_HOLD, 1, k / 390 );
  }

//Ток движения
void
StepmSetKRun( int module, int k ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_KVAL_RUN, 1, k / 390 );
  }

//Ток разгона
void
StepmSetKAccel( int module, int k ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_KVAL_ACC, 1, k / 390 );
  }

//Ток торможения
void
StepmSetKDeccel( int module, int k ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_KVAL_DEC, 1, k / 390 );
  }
  
//Пороговый ток определения, что двигатель заблокирован (ток - в мкА)
void
StepmSetStallThreshold( int module, int current ) {
  //Перейти от тока к индексу
  current /= 32250;
  current--;
  //Ограничить диапазон индекса
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_STALL_TH, 1, svILimit( current, 0, 127 ) );
  }
  
//Пороговый ток определения перегрузки по току (ток - в мкА)
void
StepmSetOverCurrent( int module, int current ) {
  //Перейти от тока к индексу
  current /= 375000;
  current--;
  //Ограничить диапазон индекса
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_OCD_TH, 1, svILimit( current, 0, 15 ) );
  }
  

//Момент перехода на полные шаги
void
StepmSetFullStepSpeed( int module, int speed ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_FS_SPD, 2, speed >> 10 );
  }

//Ускорение разгона
void
StepmSetAccel( int module, int accel ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_ACC, 2, accel );
  }

//Ускорение торможения
void
StepmSetDeccel( int module, int deccel ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_DEC, 2, deccel );
  }


void
StepmSetSpeedMax( int module, int smax ) {
  //Привести к общим значениям скорости
  //smax >>= 10;
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_MAX_SPEED, 2, smax );
  }

//Границы скорости (прямые значения)
void
StepmSetSpeedMinMax( int module, int smin, int smax ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_MIN_SPEED, 2, smin );
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_MAX_SPEED, 2, smax );
  }


//BEMF
void
StepmSetBemf( int module, int stSlp, int intSpeed, int slpAcc, int slpDec ) {
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_INT_SPD, 2, intSpeed );
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_ST_SLP, 1, stSlp );
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_FN_SLP_ACC, 1, slpAcc );
  SpiTransfer( module, SPIN_SET_PARAM|SPIN_FN_SLP_DEC, 1, slpDec );
  }


#ifdef MMM
int
PL57H( int slot ) {
  int sm;
  
  sm = Module( slot, DMT_STEP_MOTOR, 0 );
  
  StepmReset( sm );
//    vgInputs = StepmState( sm );
    
  StepmSetStepMode( sm, 2 );
  //StepmSetConfig( sm, 0x8e88 );
    
  StepmSetKHold( sm, 1000 );
  StepmSetKAccel( sm, 4000 );
  StepmSetKDeccel( sm, 4000 );
  StepmSetKRun( sm, 3000 );
  StepmSetBemf( sm, 0x29, 0xd0a, 0x3c, 0x3c );
  //StepmSetAccel( sm, 0xa0 );
  //StepmSetDeccel( sm, 0x100 );
  StepmSetFullStepSpeed( sm, 70000 );
  StepmSetSpeedMinMax( sm, 0, 0x100 );
  
  return sm;
  }
#endif


#endif // STEP_MOTOR_H
