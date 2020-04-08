
#include <Sv7Sys.h>
#include <Sv7Plc.h>
#include <spi.h>
#include <stepMotor.h>

int
vgSensors,           //Состояние входов
vgStopMoving,        //Прекращение движения
vg;

//Коды ошибок через ThrowCode
#define ERR_OK                          0
#define ERR_CAN                         1  //Ошибка связи CAN
#define ERR_TIME_OUT                    2  //Тайм-аут перемещения
#define ERR_SENSOR_STOP                 3  //Остановка по сенсору
#define ERR_STEPM_FAIL                  4  //Ошибка шагового двигателя
#define ERR_COVER                       6  //Открыта крышка
#define ERR_EMERGENCY                   7  //Нажата аварийная кнопка   
#define ERR_PICKER0_PICK_FAIL           8  //Ошибка операции захвата
#define ERR_PICKER0_ROTATION_FAIL       9  //Ошибка операции поворота
#define ERR_PICKER0_INTERFERE          10  //Захват в положении, когда он может мешать движению головы
#define ERR_WRONG_NOZZLE_INDEX         11  //Ошибочный индекс насадки

//Входы
#define INPUT_EMERGENCY        0x0004
#define SENSOR_STOP                    0x1000000

#include <stepController.c>

#define SV_SIGNATURE "sensorTuner 3"


int md0;
int mc2;

int laserId;
int laserR1;
int laserR2;
int laserAmp1;
int laserAmp2;
int newSensorId;
int rawData;



//Периодическая отправка запроса расстояния
void taskQuery() {
  while(1) {
    svWaitTick( 50 );
    ModuleCanRawWrBegin( mc2, 0x40 );
    if( newSensorId ) {
      //Команда 5 устанавливает новый идентификатор устройства
      ModuleCanRawWr8( mc2, 5 );
      ModuleCanRawWr8( mc2, newSensorId );
      newSensorId = 0;
      }
    else {
      //По умолчанию отправляем команду на чтение значений с сенсора
      //в зависимости от rawData 
      //команда 0 - преобразованные данные
      //        1 - сырые данные
      ModuleCanRawWr8( mc2, rawData ? 1 : 0 );
      }
    ModuleCanRawWrEnd( mc2 );
    }
  }



//Задача по приему сообщению и их разбору
void taskReceiv() {
  while(1) {
    if( ModuleCanRawRdCount(mc2) ) {
      laserId = ModuleCanRawRdBegin(mc2);
      ModuleCanRawRd8(mc2);
      laserR1 = ModuleCanRawRd16MS(mc2);
      laserR2 = ModuleCanRawRd16MS(mc2);
      laserAmp1 = ModuleCanRawRd8(mc2);
      laserAmp2 = ModuleCanRawRd8(mc2);
      }
    svWait();
    }
  }



int PL57H( int slot ) {
  int sm;
  
  sm = Module( slot, SVM_STEP_MOTOR_L6470, 0 );
  
  StepmReset( sm );
//    vgInputs = StepmState( sm );
    
  StepmSetStepMode( sm, 4 );
  //StepmSetConfig( sm, 0x8e88 );
    
  StepmSetKHold( sm, 1000 );
  StepmSetKAccel( sm, 4000 );
  StepmSetKDeccel( sm, 4000 );
  StepmSetKRun( sm, 3000 );
  StepmSetBemf( sm, 0x29, 0xd0a, 0x3c, 0x3c );
  //StepmSetAccel( sm, 0xa0 );
  //StepmSetDeccel( sm, 0x100 );
  StepmSetAccel( sm, 0xff );
  StepmSetDeccel( sm, 0x1ff );
  StepmSetFullStepSpeed( sm, 70000 );
  StepmSetSpeedMinMax( sm, 0, 0x100 );
  StepmSetStallThreshold( sm, 5500000 );
  StepmSetOverCurrent( sm, 6000000 );
  
  return sm;
  }

int pos, vpos, velo;



void main() {
  md0 = PL57H( 7 );

  mc2 = Module( 6, SVM_CAN_RAW, CAN_500KB );
  
  velo = 50;
  
  svCreateTask( taskQuery, 100 );
  svCreateTask( taskReceiv, 100 );
  while(1) {
    if( pos != vpos ) {
      StepmMoveAbsStep( md0, pos, vpos, velo, 10000 );
      pos = vpos;
      }
    svWait();
    }
  }
