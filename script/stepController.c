//===================================================================================================================
// Проект "Установщик B800, контроллер головы"
// Автор
//   Сибилев А.С.
// Описание
//   Управление шаговым приводом. Высокий уровень.
//==================================================================================================================

int factSpeed;
void
StepmStartMoveAbs( int module, int stepPos, int stepSpeed ) {
#ifdef OPTION_SIMULATE
  module;
  stepPos;
  stepSpeed;
    simulatorMoveModule = module;
    simulatorMoveTarget = stepPos;
    simulatorMoveSpeed = stepSpeed;
#else //OPTION_SIMULATE
  factSpeed = stepSpeed;
  //Установить новую скорость перемещения
  StepmSetSpeedMax( module, stepSpeed );
    
  //Запустить процесс передвижения
  StepmMoveAbsTo( module, stepPos );
#endif //OPTION_SIMULATE
  }
  
  
int motorStatus;

void
StepmMoveAbsStep( int module, int curPos, int pos, int velosity, int timeout ) {
  int status;
  
  curPos = svAbs( curPos - pos );
  
  if( curPos == 0 )
    //Уже на месте, возврат
    return;
    
  //Запустить процесс перемещения
  StepmStartMoveAbs( module, pos, velosity );
  
  //Ожидать истечение временного интервала или прихода механизма в заданную точку
  timeout += svTickCount + 2000;
  while(1) {
    //Получить текущее состояние контроллера двигателя
    status = StepmState( module );
    motorStatus = status;
    //Если имеются ошибки, то выдать исключение
    if( (status & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    
    //Если двигатель остановился, то успешное завершение
    if( status & 2 )
      break;
  
  
    if( svIsTimeOut(timeout) ) {
      //Тайм-аут операции
      //Остановить перемещение
      StepmSoftStop( module );
      svThrow(ERR_TIME_OUT);
      }
      
    if( vgSensors & INPUT_EMERGENCY ) {
      StepmSoftStop( module );
      return;
      }
        
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    svWait();
    }
  }





void
StepmMoveAbsStepSensor( int module, int curPos, int pos, int velocity, int timeout, int sensor ) {
  int status;
  
  //Проверить начальное состояние сенсора
  if( vgSensors & sensor ) return;
  
  curPos = svAbs( curPos - pos );
  
  if( curPos < 2 )
    //Уже на месте, возврат
    return;
    
  //Запустить процесс перемещения
  StepmStartMoveAbs( module, pos, velocity );
  
  //Ожидать истечение временного интервала или прихода механизма в заданную точку
  timeout += svTickCount + 2000;
  while(1) {
    //Получить текущее состояние контроллера двигателя
    status = StepmState( module );
    motorStatus = status;
    //Если имеются ошибки, то выдать исключение
    if( (status & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    
    //Если двигатель остановился, то успешное завершение
    if( status & 2 )
      break;
  
  
    if( svIsTimeOut(timeout) ) {
      //Тайм-аут операции
      //Остановить перемещение
      StepmSoftStop( module );
      svThrow(ERR_TIME_OUT);
      }
      
    //Проверить останов
    if( vgStopMoving || ( vgSensors & sensor ) ) {
      StepmSoftStop( module );
      return;
      }
      
    svWait();
    }
  }






void
StepmMoveAbs( int module, int curPos, int pos, int posFactor, int speed, int speedFactor ) {
   
#ifdef OPTION_SIMULATE
  module;
  curPos;
  pos;
  posFactor;
  speed;
  speedFactor;
    
    
    SimulatorMove(module, pos, speed);
    while (simulatorMoveModule){
        Wait();
    }
    return;
    
#else //OPTION_SIMULATE
  int timeout, status;
  
  curPos = svAbs( curPos - pos );
  
  if( curPos < 25 )
    //Уже на месте, возврат
    return;
    
  //Вычислить время, необходимое для перемещения
  timeout = curPos * 1000 / speed;
    
  //Запустить процесс перемещения
  StepmStartMoveAbs( module, svFactor106( pos, posFactor ), svFactor106( speed, speedFactor ) );
  
  //Ожидать истечение временного интервала или прихода механизма в заданную точку
  timeout += svTickCount + 2000;
  while(1) {
    //Получить текущее состояние контроллера двигателя
    status = StepmState( module );
    motorStatus = status;
    //Если имеются ошибки, то выдать исключение
    if( (status & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    
    //Если двигатель остановился, то успешное завершение
    if( status & 2 )
      break;
  
  
    if( svIsTimeOut( timeout ) ) {
      //Тайм-аут операции
      //Остановить перемещение
      StepmSoftStop( module );
      svThrow(ERR_TIME_OUT);
      }
      
     if (vgSensors & INPUT_EMERGENCY){
      StepmSoftStop( module );
      return;
    }
        
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    svWait();
    }
#endif //OPTION_SIMULATE
  }
      

void
StepmMoveAbsSensor( int module, int curPos, int pos, int posFactor, int speed, int speedFactor, int sensor ) {
#ifdef OPTION_SIMULATE
  module;
  curPos;
  pos;
  posFactor;
  speed;
  speedFactor;
  sensor;
  WaitTick( 300 );
#else //OPTION_SIMULATE
  int timeout, status;
  
  //Проверить начальное состояние сенсора
  if( vgSensors & sensor ) return;
  
  curPos = svAbs( curPos - pos );
  
  if( curPos < 25 )
    //Уже на месте, возврат
    return;
    
  //Вычислить время, необходимое для перемещения
  timeout = curPos * 1000 / speed;
    
  //Запустить процесс перемещения
  StepmStartMoveAbs( module, svFactor106( pos, posFactor ), svFactor106( speed, speedFactor ) );
  
  //Ожидать истечение временного интервала или прихода механизма в заданную точку
  timeout += svTickCount + 2000;
  while(1) {
    //Получить текущее состояние контроллера двигателя
    status = StepmState( module );
    motorStatus = status;
    //Если имеются ошибки, то выдать исключение
    if( (status & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    
    //Если двигатель остановился, то успешное завершение
    if( status & 2 )
      break;
  
  
    if( svIsTimeOut( timeout ) ) {
      //Тайм-аут операции
      //Остановить перемещение
      StepmSoftStop( module );
      svThrow(ERR_TIME_OUT);
      }
      
     if (vgSensors & INPUT_EMERGENCY){
      StepmSoftStop( module );
      return;
    }
        
    //Проверить останов
    if( vgStopMoving || ( vgSensors & sensor ) ) {
      StepmSoftStop( module );
      return;
      }
      
    svWait();
    }
#endif //OPTION_SIMULATE
  }
      
      
      
int
StepmGetPosition( int module, int posFactor ) {
  return svDeFactor106( StepmGetPos( module ), posFactor );
  }







//Выполнить перемещение на постоянной скорости
void
StepmSlowMoveAbs( int module, int *curPos, int pos, int posFactor, int speed, int speedFactor ) {
  int timeout, delta, dir;
  
  delta = svAbs( *curPos - pos );
  
  if( delta < 25 )
    //Уже на месте, возврат
    return;
  
  //Вычислить время, необходимое для перемещения
  timeout = delta * 1000 / speed;
  
  //Определить направление
  if( *curPos > pos ) dir = 0;
  else                dir = 1;
  
  //Включить двигатель
  StepmRun( module, dir, svFactor103( speed, speedFactor ) );
  
  timeout += 2000 + svTickCount;
  
  while( !svIsTimeOut(timeout) ) {
    svWait();
        
        
    if (vgSensors & INPUT_EMERGENCY){
      StepmSoftStop( module );
      return;
    }
    
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    //Проверить достижение границы
    *curPos = StepmGetPosition( module, posFactor );
    if( dir && pos <= *curPos ) {
      StepmSoftStop( module );
      return;
      }

    if( dir == 0 && pos >= *curPos ) {
      StepmSoftStop( module );
      return;
      }
      
    //Получить текущее состояние контроллера двигателя
    //Если имеются ошибки, то выдать исключение
    if( (StepmState( module ) & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      StepmSoftStop( module );
      svThrow(ERR_STEPM_FAIL);
      }
    }
  
  //Остановить перемещение
  StepmSoftStop( module );
  svThrow(ERR_TIME_OUT);
  }
  


#ifndef SLOW_TIME
#define SLOW_TIME            2000  //Время движения на низкой скорости (до перехода на среднюю)
#endif

#ifndef MIDDLE_TIME
#define MIDDLE_TIME           2000  //Время движения на средней скорости (до перехода на высокую)
#endif

//Перемещение 
void
StepmMovingWhileStop( int module, int dir, int velosityStart, int velosityMiddle, int velosityFast, int speedFactor, int minPos, int maxPos, int posFactor ) {
#ifdef OPTION_SIMULATE
  module;
  dir;
  velosityStart;
  velosityMiddle;
  velosityFast;
  speedFactor;
  minPos;
  maxPos;
#endif
  int timeout;
  
  //Проверить позицию
  if( dir && maxPos <= StepmGetPosition( module, posFactor ) )
    return;
    
  if( dir == 0 && minPos >= StepmGetPosition( module, posFactor ) )
    return;
  
  //1-е движение - на минимальной скорости
  
  //Включить двигатель
  StepmRun( module, dir, svFactor103( velosityStart, speedFactor ) );
  
  timeout = SLOW_TIME + svTickCount;
  
  //Ожидать истечение времени или срабатывание останова
  while( !svIsTimeOut( timeout ) ) {
    svWait();
    
     if (vgSensors & INPUT_EMERGENCY){
      StepmSoftStop( module );
      return;
    }
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    //Проверить достижение границы
    if( dir && maxPos <= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }

    if( dir == 0 && minPos >= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }
      
    //Получить текущее состояние контроллера двигателя
    //Если имеются ошибки, то выдать исключение
    if( (StepmState( module ) & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    }
    
  //Переходим на среднюю скорость
  StepmRun( module, dir, svFactor103( velosityMiddle, speedFactor ) );
  
  timeout = MIDDLE_TIME + svTickCount;
  
  //Ожидать истечение времени или срабатывание останова
  while( !svIsTimeOut( timeout ) ) {
    svWait();
    
    if( vgSensors & INPUT_EMERGENCY ) {
      StepmSoftStop( module );
      return;
      }
        
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    //Проверить достижение границы
    if( dir && maxPos <= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }

    if( dir == 0 && minPos >= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }
      
    //Получить текущее состояние контроллера двигателя
    //Если имеются ошибки, то выдать исключение
    if( (StepmState( module ) & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    }
  
  //Переходим на высокую скорость
  StepmRun( module, dir, svFactor103( velosityFast, speedFactor ) );
  
  //Ожидать срабатывание останова
  while(1) {
    svWait();
    
    if( vgSensors & INPUT_EMERGENCY ) {
      StepmSoftStop( module );
      return;
      }
        
    //Проверить останов
    if( vgStopMoving ) {
      StepmSoftStop( module );
      return;
      }
      
    //Проверить достижение границы
    if( dir && maxPos <= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }

    if( dir == 0 && minPos >= StepmGetPosition( module, posFactor ) ) {
      StepmSoftStop( module );
      return;
      }
      
    //Получить текущее состояние контроллера двигателя
    //Если имеются ошибки, то выдать исключение
    if( (StepmState( module ) & 0x7c00) != 0x7c00 ) {
      //Возникла ошибка в контроллере - исключение
      svThrow(ERR_STEPM_FAIL);
      }
    }
  }





void
StepmMoveToSensorStep( int module, int dir, int velosity, int timeout, int sensor ) {
  //Заводим таймер
  timeout += svTickCount;
  
  //Включить двигатель
  StepmRun( module, dir, velosity );
  
  //Ожидать истечение времени или срабатывание сенсора
  while(1) {
    //Получить текущее состояние контроллера двигателя
    motorStatus = StepmState( module );
    
    if( vgSensors & INPUT_EMERGENCY ) {
      StepmSoftStop( module );
      svThrow(ERR_EMERGENCY);
      break;
      }
    //Проверить сенсор 
    if( vgSensors & sensor || vgStopMoving ) {
      //Сработал выходной сенсор или сенсор останова
      break;
      }
      
    //Проверить сенсор останова
    if( vgSensors & SENSOR_STOP ) {
      //Сработал сенсор останова
      StepmSoftStop( module );
      svThrow(ERR_SENSOR_STOP);
      }
      
    //Проверить таймер
    if( svIsTimeOut( timeout ) ) {
      //Тайм-аут операции
      StepmSoftStop( module );
      svThrow(ERR_TIME_OUT);
      }
      
    svWait();
    }
    
  //Остановить двигатели
  StepmSoftStop( module );
  }

      
      
  


void
StepmMoveToSensor( int module, int dir, int velosity, int speedFactor, int timeout, int sensor ) {
#ifdef OPTION_SIMULATE
  module;
  dir;
  velosity;
  speedFactor;
  timeout;
  sensor;
  WaitTick( 500 );
#else //OPTION_SIMULATE
  //Преобразовать скорость
  velosity = svFactor103( velosity, speedFactor );
  
  //Выполнить движение
  StepmMoveToSensorStep( module, dir, velosity, timeout, sensor );
#endif //OPTION_SIMULATE
  }
  
  
int
StepmReferenceStep( int module, int dir, int velosity, int fineVelosity, int timeout, int sensorOn, int sensorOff ) {
  int ndir;
  
  //Вычислить противоположное направление
  ndir = !dir;

  //Установить новую скорость перемещения
  StepmSetSpeedMax( module, velosity * 2 );
    
  //На быстрой скорости ищем границу
  if( vgSensors & sensorOn ) {
    //Искать в противоположном направлении (съехать с сенсора)
    StepmMoveToSensorStep( module, ndir, velosity, timeout, sensorOff );
    //Снова заехать на сенсор (путь 2000мкм)
    timeout = 20000000 / velosity;
    StepmMoveToSensorStep( module, dir, velosity, timeout, sensorOn );
    }
  else
    //Искать в прямом направлении
    StepmMoveToSensorStep( module, dir, velosity, timeout, sensorOn );
    
  //Границу нашли, выполнить точную подстройку
  //Точная подстройка всегда выполняется в одном противоположном направлении
  timeout = 20000000 / fineVelosity;
  StepmMoveToSensorStep( module, ndir, fineVelosity, timeout, sensorOff );

  //Фиксировать данное положение как нулевое
  svWaitTick(100);
  //Новая возможность - фиксация положения, при котором сработал датчик дома
  //Эта возможность нужна в приложении определения высоты по упору
  ndir = StepmGetPos( module );
  //Теперь установить нулевую позицию
  StepmSetPos( module, 0 );
  return ndir;
  }



void
StepmReference( int module, int dir, int velosity, int fineVelosity, int speedFactor, int timeout, int sensorOn, int sensorOff ) {
#ifdef OPTION_SIMULATE
  module;
  dir;
  velosity;
  fineVelosity;
  speedFactor;
  timeout;
  sensorOn;
  sensorOff;
   
    SimulatorMoveReference(module, velosity, sensorOn);
    while(simulatorMoveModule)
        Wait();
    return;
#else //OPTION_SIMULATE
  //Преобразовать скорость
  velosity = svFactor103( velosity, speedFactor );
  fineVelosity = svFactor103( fineVelosity, speedFactor );
  
  //Выполнить референс
  StepmReferenceStep( module, dir, velosity, fineVelosity, timeout, sensorOn, sensorOff );
#endif //OPTION_SIMULATE
  }



int
PosEqual( int pos1, int pos2 ) {
  if( svAbs( pos1 - pos2 ) < 50 ) return 1;
  return 0;
  }

