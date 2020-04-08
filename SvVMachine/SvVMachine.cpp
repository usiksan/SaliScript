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
    Виртуальная машина для исполнения байт-кода.
    Виртуальная машина содержит в себе память программ, память данных
    и набор виртуальных процессоров.
*/
#include "SvVMachine/SvVMachine.h"
#include "SvVMachine/Sv7Sys.h"
#include "SvVMachine/SvVmByteCode.h"
#include <string.h>
#include <stdlib.h>

//#define SV_DEBUG_ON

#ifdef SV_DEBUG_ON
#include <QDebug>
#define SV_DEBUG(x) x
#else
#define SV_DEBUG(x)
#endif

SvVMachine::SvVMachine() :
  mMemory(nullptr),
  mMemorySize(0),
  mStack(0),
  mProg(nullptr),
  mProgSize(0),
  mVpuList(nullptr),
  mVpuCount(0),
  mVpuMax(0)
  {
  }

SvVMachine::~SvVMachine()
  {
  }

void SvVMachine::processing(int timeOffset)
  {
  //Инкремент счетчика времени
  memSet( nullptr, 1, memGet(nullptr,1) + timeOffset );

  //Для всех VPU выполнить один шаг
  for( int i = 0; i < mVpuCount; i++ ) {
    SV_DEBUG(qDebug() << "**** vpu" << i;)
    executeCore( mVpuList + i );
    }

  }



//Выполнить внешний метод
//Возвращает истину, если нужно ждать (переключиться на другую задачу)
bool SvVMachine::executeMethod(SvVmVpu *vpu, int methodId)
  {
  switch( methodId ) {
    case VPS_CREATETASK :
      //import VPS_CREATETASK   int CreateTask( void (*fun)(), int stackSize );
      //Создаем новую задачу
      if( mVpuCount < mVpuMax ) {
        //Место под новую задачу есть, организовать
        mVpuList[mVpuCount].enter( VPU_GET_FUN_PARAM(1), mStack );
        mStack -= VPU_GET_FUN_PARAM(0);
        //Вернуть id VPU
        VPU_SET_FUN_RESULT( 2, mVpuCount++ );
        SV_DEBUG(qDebug() << "**** task created" << mVpuCount;)
        }
      else {
        //Места под новую задачу нет, вернуть 0
        VPU_SET_FUN_RESULT( 2, 0 );
        }
      break;

    case VPS_THROW_CODE :
      //import VPS_THROW_CODE   int ThrowCode();
      //Вернуть текущий код исключения
      VPU_SET_FUN_RESULT( 0, vpu->mThrow );
      break;

    case VPS_ABS :
      //1             0
      //int  Abs( int val );
      VPU_SET_FUN_RESULT( 1, abs(VPU_GET_FUN_PARAM(0)) );
      break;

    case VPS_VERSION :
      //0
      //int VpuVersion();
      VPU_SET_FUN_RESULT( 0, VPU_VERSION7 );
      break;

    case VPS_TICKET00 :
      //                 0
      //void svTicket0( int ticketId );
      if( mTicket[0] ) return true;
      mTicket[0] = VPU_GET_FUN_PARAM(0);
      break;

    case VPS_TICKET10 :
    case VPS_TICKET01 :
      //                 1             0
      //void svTicket1( int ticketId, int p0 );
      //void svTicket0r1( int ticketId, int *r0 );
      if( mTicket[0] ) return true;
      mTicket[0] = VPU_GET_FUN_PARAM(1);
      mTicket[1] = VPU_GET_FUN_PARAM(0);
      break;

    case VPS_TICKET20 :
    case VPS_TICKET11 :
      //                 2             1       0
      //void svTicket2( int ticketId, int p0, int p1 );
      //void svTicket1r1( int ticketId, int p0, int *r0 );
      if( mTicket[0] ) return true;
      mTicket[0] = VPU_GET_FUN_PARAM(2);
      mTicket[1] = VPU_GET_FUN_PARAM(1);
      mTicket[2] = VPU_GET_FUN_PARAM(0);
      break;

    case VPS_TICKET40 :
    case VPS_TICKET22 :
      //                 4             3       2       1       0
      //void svTicket4( int ticketId, int p0, int p1, int p2, int p3 );
      //void svTicket2r2( int ticketId, int p0, int p1, int *r0, int *r1 );
      if( mTicket[0] ) return true;
      mTicket[0] = VPU_GET_FUN_PARAM(4);
      mTicket[1] = VPU_GET_FUN_PARAM(3);
      mTicket[2] = VPU_GET_FUN_PARAM(2);
      mTicket[3] = VPU_GET_FUN_PARAM(1);
      mTicket[4] = VPU_GET_FUN_PARAM(0);
      break;

    case VPS_TICKET80 :
    case VPS_TICKET44 :
    case VPS_TICKET26 :
      //                 8             7       6       5       4       3       2       1       0
      //void svTicket8( int ticketId, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7 );
      //void svTicket4r4( int ticketId, int p0, int p1, int p2, int p3, int *r0, int *r1, int *r2, int *r3 );
      //void svTicket2r6( int ticketId, int p0, int p1, int *r0, int *r1, int *r2, int *r3, int *r4, int *r5 );
      if( mTicket[0] ) return true;
      mTicket[0] = VPU_GET_FUN_PARAM(8);
      mTicket[1] = VPU_GET_FUN_PARAM(7);
      mTicket[2] = VPU_GET_FUN_PARAM(6);
      mTicket[3] = VPU_GET_FUN_PARAM(5);
      mTicket[4] = VPU_GET_FUN_PARAM(4);
      mTicket[5] = VPU_GET_FUN_PARAM(3);
      mTicket[6] = VPU_GET_FUN_PARAM(2);
      mTicket[7] = VPU_GET_FUN_PARAM(1);
      mTicket[8] = VPU_GET_FUN_PARAM(0);
      break;

    case VPS_WAIT_TIME_OUT :
      //                     0
      //void svWaitTimeOut( int time );
      return (VPU_GET_FUN_PARAM(0) - memGet( vpu, 1 )) > 0;

    case VPS_IS_TIME_OUT :
      // 1                 0
      //int  svIsTimeOut( int time );
      VPU_SET_FUN_RESULT( 1, (VPU_GET_FUN_PARAM(0) - memGet( vpu, 1 )) > 0 ? 0 : 1 );
      break;


    case VPS_IMIN :
      //2              1       0
      //int  IMin( int v1, int v2 );
      VPU_SET_FUN_RESULT( 2, svIMin( VPU_GET_FUN_PARAM(1), VPU_GET_FUN_PARAM(0) ) );
      break;

    case VPS_IMAX :
      //2              1       0
      //int  IMax( int v1, int v2 );
      VPU_SET_FUN_RESULT( 2, svIMax( VPU_GET_FUN_PARAM(1), VPU_GET_FUN_PARAM(0) ) );
      break;

    case VPS_UMIN :
      //2              1       0
      //int  UMin( int v1, int v2 );
      VPU_SET_FUN_RESULT( 2, static_cast<int>(svUMin( static_cast<unsigned>(VPU_GET_FUN_PARAM(1)), static_cast<unsigned>(VPU_GET_FUN_PARAM(0)) )) );
      break;

    case VPS_UMAX :
      //2              1       0
      //int  UMax( int v1, int v2 );
      VPU_SET_FUN_RESULT( 2, static_cast<int>(svUMax( static_cast<unsigned>(VPU_GET_FUN_PARAM(1)), static_cast<unsigned>(VPU_GET_FUN_PARAM(0)) )) );
      break;

    case VPS_ILIMIT :
      //3                 2          1           0
      //int  ILimit( int val, int minVal, int maxVal );
      VPU_SET_FUN_RESULT( 3, svILimit( VPU_GET_FUN_PARAM(2), VPU_GET_FUN_PARAM(1), VPU_GET_FUN_PARAM(0) ) );
      break;

    case VPS_FACTOR106 :
      //2                   1          0
      //int  Factor106( int val, int factor );
      VPU_SET_FUN_RESULT( 2, static_cast<int>( (static_cast<double>(VPU_GET_FUN_PARAM(1))) * ((static_cast<double>(VPU_GET_FUN_PARAM(0))) / 1000.0) / 1000.0 ) );
      break;

    case VPS_FACTOR103 :
      //2                   1          0
      //int  Factor103( int val, int factor );
      VPU_SET_FUN_RESULT( 2, static_cast<int>( (static_cast<double>(VPU_GET_FUN_PARAM(1))) * ((static_cast<double>(VPU_GET_FUN_PARAM(0))) / 1000.0) ) );
      break;

    case VPS_DEFACTOR106 :
      //2                   1          0
      //int  DeFactor106( int val, int factor );
      VPU_SET_FUN_RESULT( 2, static_cast<int>( (static_cast<double>(VPU_GET_FUN_PARAM(1))) * 1000.0 / ((static_cast<double>(VPU_GET_FUN_PARAM(0))) / 1000.0) ) );
      break;

    case VPS_DEFACTOR103 :
      //2                   1          0
      //int  DeFactor104( int val, int factor );
      VPU_SET_FUN_RESULT( 2, static_cast<int>( (static_cast<double>(VPU_GET_FUN_PARAM(1))) / ((static_cast<double>(VPU_GET_FUN_PARAM(0))) / 1000.0) ) );
      break;

    case VPS_RND :
      //2              1           0
      //int  Random( int minVal, int maxVal )
      //Random( Value(mSp+1), Value(mSp) )
      VPU_SET_FUN_RESULT( 2, rand() );
      break;

    }
  return false;
  }







//Установить новую программу
void SvVMachine::setProgramm( const SvVmCode *code )
  {
  mVpuCount = 0;
  mProg = code;
  mProgSize = mProg == nullptr ? 0 : progSize();
  }




//Все очистить и убрать все виртуальные процессоры
void SvVMachine::reset()
  {
  mVpuCount = 0;
  mStack = mMemorySize - 1;

  //Подготовить память
  if( mMemorySize )
    memset( mMemory, 0, static_cast<unsigned>(mMemorySize) * sizeof(int) );
  }






//Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
void SvVMachine::restart(bool run)
  {
  reset();

  if( mMemory == nullptr || mProg == nullptr || mVpuList == nullptr )
    return;

  //Установить размер памяти программ
  mProgSize = vpuProgSize();

  if( mProgSize > 0 ) {
    //Исполнить таблицу конструкторов и инициализаторов
    int table = initTable();
    for( int i = 0; value3( nullptr, table + i ) != 0; i += 7 )
      memSet( nullptr, value3( nullptr, table + i), value4( nullptr, table + i + 3) );

    if( enterPoint() > 0 && enterPoint() < mProgSize ) {
      //Образовать один процессор
      mVpuCount = 1;
      mVpuList[0].enter( enterPoint(), mStack );
      if( run )
        mVpuList[0].debugRun();
      else
        mVpuList[0].debugRunUntil( 0, mProgSize );
      mStack -= stackSize0();
      }
    }
  }




//Отладка - пуск
void SvVMachine::debugRun(int vpu)
  {
  //Проверить границы VPU
  if( vpu < 0 || vpu >= mVpuCount ) return;

  //Изменить флаг исполнения
  mVpuList[vpu].debugRun();
  }




//Отладка - стоп (пауза)
void SvVMachine::debugPause(int vpu)
  {
  debugRunUntil( vpu, 0, mProgSize );
  }




//Исполнять, пока находится в диапазоне и bp меньше debugBp (trace)
void SvVMachine::debugRunTrace(int vpu, int start, int stop)
  {
  //Проверить границы VPU
  if( vpu < 0 || vpu >= mVpuCount ) return;

  mVpuList[vpu].debugRunTrace( start, stop );
  }



//Исполнять, пока находится в диапазоне и bp меньше debugBp (step)
void SvVMachine::debugRunStep(int vpu, int start, int stop)
  {
  //Проверить границы VPU
  if( vpu < 0 || vpu >= mVpuCount ) return;

  mVpuList[vpu].debugRunStep( start, stop );
  }




//Исполнять, пока находится вне диапазона (break point)
void SvVMachine::debugRunUntil(int vpu, int start, int stop)
  {
  //Проверить границы VPU
  if( vpu < 0 || vpu >= mVpuCount ) return;

  mVpuList[vpu].debugRunUntil( start, stop );
  }






#define MEM( idx, val ) memSet( vpu, idx, val )
#define VAL( idx )      memGet( vpu, idx )
#define PARAM8          value1( vpu, vpu->mIp + 1 )
#define PARAM8_8        value1( vpu, vpu->mIp + 2 )
#define PARAM16         value2( vpu, vpu->mIp + 1 )
#define PARAM24         value3( vpu, vpu->mIp + 1 )
#define PARAM32         value4( vpu, vpu->mIp + 1 )


void SvVMachine::executeCore(SvVmVpu *vpu)
  {
  int tmp;
  int count = 0;
  while( vpu->mIp ) {
    if( count++ > 10000 )
      break;
    //Проверить необходимость останова
    if( !vpu->isRun() )
      return;

    //Проверить допустимость указателя программы
    if( vpu->mIp < 0 || vpu->mIp >= mProgSize ) {
      progFail( vpu, vpu->mIp, 1 );
      vpu->mIp = 0;
      return;
      }



    //Исполнение команд
    switch( static_cast<SvVmByteCode>(mProg[vpu->mIp]) )
      {
      //Команда останова
      case VBC1_HALT :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_HALT";)
        vpu->mIp = 0;
        break;

      //Stack operations read-write
      case VBC1_PUSH_TOS :        //stack[sp-1] = stack[sp--];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PUSH_TOS" << VAL(vpu->mSp);)
        MEM( vpu->mSp-1, VAL(vpu->mSp) );
        vpu->mSp--;
        vpu->mIp++;
        break;

      case VBC1_PUSH_0 :          //stack[--sp] = 0
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PUSH_0";)
        MEM( --(vpu->mSp), 0 );
        vpu->mIp++;
        break;

      case VBC1_PUSH_1 :          //stack[--sp] = 1
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PUSH_1";)
        MEM( --(vpu->mSp), 1 );
        vpu->mIp++;
        break;

      case VBC1_PUSH_2 :          //stack[--sp] = 2
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PUSH_2";)
        MEM( --(vpu->mSp), 2 );
        vpu->mIp++;
        break;

      case VBC2_PUSH_CONST :      //stack[--sp] = const8
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_PUSH_CONST" << PARAM8;)
        MEM( --(vpu->mSp), PARAM8 );
        vpu->mIp += 2;
        break;

      case VBC3_PUSH_CONST :      //stack[--sp] = const16
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC3_PUSH_CONST" << PARAM16;)
        MEM( --(vpu->mSp), PARAM16 );
        vpu->mIp += 3;
        break;

      case VBC4_PUSH_CONST :      //stack[--sp] = const24
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_PUSH_CONST" << PARAM24;)
        MEM( --(vpu->mSp), PARAM24 );
        vpu->mIp += 4;
        break;

      case VBC5_PUSH_CONST :      //stack[--sp] = const32; Загрузка обычного целого
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC5_PUSH_CONST" << PARAM32;)
        MEM( --(vpu->mSp), PARAM32 );
        vpu->mIp += 5;
        break;

      case VBC1_ADD_1 :           //stack[sp] += 1;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ADD_1" << VAL(vpu->mSp) << 1;)
        MEM( vpu->mSp, VAL(vpu->mSp) + 1 );
        vpu->mIp++;
        break;

      case VBC1_ADD_2 :           //stack[sp] += 2;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ADD_2" << VAL(vpu->mSp) << 2;)
        MEM( vpu->mSp, VAL(vpu->mSp) + 2 );
        vpu->mIp++;
        break;

      case VBC1_ADD_3 :           //stack[sp] += 3;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ADD_3" << VAL(vpu->mSp) << 3;)
        MEM( vpu->mSp, VAL(vpu->mSp) + 3 );
        vpu->mIp++;
        break;

      case VBC1_ADD_4 :           //stack[sp] += 4;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ADD_4" << VAL(vpu->mSp) << 4;)
        MEM( vpu->mSp, VAL(vpu->mSp) + 4 );
        vpu->mIp++;
        break;

      case VBC2_ADD_CONST :       //stack[sp] += const8
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_ADD_CONST" << VAL(vpu->mSp) << PARAM8;)
        MEM( vpu->mSp, VAL(vpu->mSp) + PARAM8 );
        vpu->mIp += 2;
        break;

      case VBC2_PUSH_B_OFFSET :   //stack[--sp] = bp + offset; Загрузка адреса локальной переменной
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_PUSH_B_OFFSET" << vpu->mBp << PARAM8;)
        MEM(--(vpu->mSp), vpu->mBp + PARAM8 );
        vpu->mIp += 2;
        break;

      case VBC4_PUSH_GLOBAL :     //stack[--sp] = global[param]
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_PUSH_GLOBAL" << PARAM24 << VAL(PARAM24);)
        MEM(--(vpu->mSp), VAL(PARAM24) );
        vpu->mIp += 4;
        break;

      case VBC2_PUSH_LOCAL :      //stack[--sp] = global[bp+offset]
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_PUSH_LOCAL" << PARAM8 << VAL(vpu->mBp + PARAM8);)
        MEM(--(vpu->mSp), VAL(vpu->mBp + PARAM8) );
        vpu->mIp += 2;
        break;

        //Разместить код исключения в стек
      case VBC1_PUSH_THROW :      //stack[--sp] = mThrow
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PUSH_THROW" << vpu->mThrow;)
        MEM(--(vpu->mSp), vpu->mThrow );
        vpu->mIp++;
        break;


      case VBC1_LOAD :            //stack[sp] = global[ stack[sp] ]; //Косвенная загрузка
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_LOAD" << VAL(vpu->mSp) << VAL( VAL(vpu->mSp) );)
        MEM( vpu->mSp, VAL( VAL(vpu->mSp) ) );
        vpu->mIp++;
        break;

      case VBC1_STORE :           //global[ stack[sp+1] ] = stack[sp];
                                  //stack[sp+1] = stack[sp++];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STORE" << VAL(vpu->mSp + 1) << VAL(vpu->mSp);)
        MEM( VAL(vpu->mSp + 1), VAL(vpu->mSp) );
        MEM( vpu->mSp + 1, VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_POP :             //global[ stack[sp+1] ] = stack[sp];
                                  //sp += 2;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_POP" << VAL(vpu->mSp + 1) << VAL(vpu->mSp);)
        MEM( VAL(vpu->mSp + 1), VAL(vpu->mSp) );
        vpu->mSp += 2;
        vpu->mIp++;
        break;

      case VBC2_POP_RESULT :      //stack[bp+param] = stack[sp++];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_POP_RESULT" << vpu->mBp << PARAM8 << VAL(vpu->mSp);)
        MEM( vpu->mBp + PARAM8, VAL(vpu->mSp++) );
        vpu->mIp += 2;
        break;

      //Stack ip manipulations [Манипуляции с указателем стека]
      case VBC1_STACK_DN1 :       //sp--
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_DN1";)
        vpu->mSp--;
        vpu->mIp++;
        break;

      case VBC1_STACK_DN2 :       //sp -= 2
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_DN2";)
        vpu->mSp -= 2;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP1 :       //sp++
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP1";)
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP2 :       //sp += 2
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP2";)
        vpu->mSp += 2;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP3 :       //sp += 3
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP3";)
        vpu->mSp += 3;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP4 :       //sp += 4
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP4";)
        vpu->mSp += 4;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP5 :       //sp += 5
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP5";)
        vpu->mSp += 5;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP6 :       //sp += 6
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP6";)
        vpu->mSp += 6;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP7 :       //sp += 7
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP7";)
        vpu->mSp += 7;
        vpu->mIp++;
        break;

      case VBC1_STACK_UP8 :       //sp += 8
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_STACK_UP8";)
        vpu->mSp += 8;
        vpu->mIp++;
        break;

      case VBC2_STACK :           //sp += param; (param - число со знаком)
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_STACK" << PARAM8;)
        vpu->mSp += PARAM8;
        vpu->mIp += 2;
        break;

      case VBC3_STACK :           //sp += param; (param - число со знаком)
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC3_STACK" << PARAM16;)
        vpu->mSp += PARAM16;
        vpu->mIp += 3;
        break;



      //Integer operations
      case VBC1_ADD :             //global[sp+1] = global[sp+1] + global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ADD" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) + VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_SUB :             //global[sp+1] = global[sp+1] - global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_SUB" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) - VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_AND :             //global[sp+1] = global[sp+1] & global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_AND" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) & VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_OR :              //global[sp+1] = global[sp+1] | global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_OR" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) | VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_XOR :             //global[sp+1] = global[sp+1] ^ global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_XOR" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) ^ VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_LSHIFT :          //global[sp+1] = global[sp+1] << global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_LSHIFT" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) << VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_RSHIFT :          //global[sp+1] = global[sp+1] >> global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_RSHIFT" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) >> VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_EQU :             //global[sp+1] = global[sp+1] == global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_EQU" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) == VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_NOT_EQU :         //global[sp+1] = global[sp+1] != global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_NOT_EQU" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) != VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_LESS :            //global[sp+1] = global[sp+1] < global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_LESS" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) < VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_LESS_EQU :        //global[sp+1] = global[sp+1] <= global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_LESS_EQU" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) <= VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_GREAT :           //global[sp+1] = global[sp+1] > global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_GREAT" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) > VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_GREAT_EQU :       //global[sp+1] = global[sp+1] >= global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_GREAT_EQU" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) >= VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_MUL :             //global[sp+1] = global[sp+1] * global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_MUL" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) * VAL(vpu->mSp) );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_DIV :             //global[sp+1] = global[sp+1] / global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_DIV" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        tmp = VAL(vpu->mSp);
        if( tmp == 0 ) tmp = 1;
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) / tmp );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_MOD :             //global[sp+1] = global[sp+1] % global[sp]; sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_MOD" << VAL(vpu->mSp+1) << VAL(vpu->mSp);)
        tmp = VAL(vpu->mSp);
        if( tmp == 0 ) tmp = 1;
        MEM( vpu->mSp+1, VAL(vpu->mSp+1) % tmp );
        vpu->mSp++;
        vpu->mIp++;
        break;

      case VBC1_LNOT :            //global[sp] = !global[sp]
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_LNOT" << VAL(vpu->mSp);)
        MEM( vpu->mSp, !VAL(vpu->mSp) );
        vpu->mIp++;
        break;

      case VBC1_NOT :             //global[sp] = ~global[sp]
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_NOT" << VAL(vpu->mSp);)
        MEM( vpu->mSp, ~VAL(vpu->mSp) );
        vpu->mIp++;
        break;

      case VBC1_NEG :             //global[sp] = -global[sp]
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_NEG" << VAL(vpu->mSp);)
        MEM( vpu->mSp, -VAL(vpu->mSp) );
        vpu->mIp++;
        break;

        //Фрейм глобальной функции
        // результат
        // параметр 0
        // параметр 1
        // ...
        // bp адрес фрейма вызывающей функции
        // tm маска воспринимаемых исключений
        // tp указатель this
        // ip адрес возврата

        //Возврат из функции
      case VBC1_RETURN :          //sp = bp + 4;
                                  //ip = global[bp]; tm = global[bp+1]; tp = global[bp+2]; bp = global[bp+3];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_RETURN";)
        vpu->mSp = vpu->mBp + 3;
        vpu->mIp = mMemory[vpu->mBp];
        vpu->mTm = mMemory[vpu->mBp+1];
        vpu->mBp = mMemory[vpu->mBp+2];
        break;


      case VBC1_ALLOC_RESULT :    //stack[sp-1] = stack[sp--];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_ALLOC_RESULT" << VAL(vpu->mSp);)
        MEM( vpu->mSp-1, VAL(vpu->mSp) );
        MEM( vpu->mSp--, 0 );
        vpu->mIp++;
        break;

        //Вызов функции
        //addr = operand
        //global[--sp] = bp;
        //global[--sp] = tm; tm = 0;
        //global[--sp] = ip; ip = addr;
        //bp = sp;
      case VBC2_CALL :            //call( param );
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC2_CALL" << PARAM8;)
        tmp = PARAM8;
        if( call( vpu, tmp, 2 ) ) return;
        break;

      case VBC1_CALL0 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL0";)
        if( call( vpu, 0, 1 ) ) return;
        break;

      case VBC1_CALL1 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL1";)
        if( call( vpu, 1, 1 ) ) return;
        break;

      case VBC1_CALL2 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL2";)
        if( call( vpu, 2, 1 ) ) return;
        break;

      case VBC1_CALL3 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL3";)
        if( call( vpu, 3, 1 ) ) return;
        break;

      case VBC1_CALL4 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL4";)
        if( call( vpu, 4, 1 ) ) return;
        break;

      case VBC1_CALL5 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL5";)
        if( call( vpu, 5, 1 ) ) return;
        break;

      case VBC1_CALL6 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL6";)
        if( call( vpu, 6, 1 ) ) return;
        break;

      case VBC1_CALL7 :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CALL7";)
        if( call( vpu, 7, 1 ) ) return;
        break;

        //Инкремент
      case VBC1_INC :             //global[ stack[sp++] ]++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp++ );
        if( tmp > 0 && tmp < mMemorySize )
          mMemory[tmp]++;
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_INC :             //global[ stack[sp++] ] += param24
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp++ );
        if( tmp > 0 && tmp < mMemorySize )
          mMemory[tmp] += PARAM24;
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;

        //Декремент
      case VBC1_DEC :             //global[ stack[sp++] ]--;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp++ );
        if( tmp > 0 && tmp < mMemorySize )
          mMemory[tmp]--;
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_DEC :             //global[ stack[sp++] ] -= param24
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp++ );
        if( tmp > 0 && tmp < mMemorySize )
          mMemory[tmp] -= PARAM24;
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;

        //Постинкремент
      case VBC1_POST_INC :        //stack[sp] = global[ stack[sp] ]++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_POST_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, mMemory[tmp]++ );
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_POST_INC :        //tmp = stack[sp]; stack[sp] = global[ tmp ]; global[tmp] += param24
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_POST_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize ) {
          MEM( vpu->mSp, mMemory[tmp] );
          mMemory[tmp] += PARAM24;
          }
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;

        //Постдекремент
      case VBC1_POST_DEC :        //stack[sp] = global[ stack[sp] ]--;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_POST_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, mMemory[tmp]-- );
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_POST_DEC :        //tmp = stack[sp]; stack[sp] = global[ tmp ]; global[tmp] -= param24
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_POST_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize ) {
          MEM( vpu->mSp, mMemory[tmp] );
          mMemory[tmp] -= PARAM24;
          }
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;


        //Прединкремент
      case VBC1_PRED_INC :        //stack[sp] = ++global[ stack[sp] ];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PRED_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, ++mMemory[tmp] );
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_PRED_INC :        //stack[sp] = global[ stack[sp] ] += param24;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_PRED_INC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, mMemory[tmp] += PARAM24 );
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;


        //Преддекремент
      case VBC1_PRED_DEC :        //stack[sp] = --global[ stack[sp] ];
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_PRED_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, --mMemory[tmp] );
        else memFail( vpu, tmp );
        vpu->mIp++;
        break;

      case VBC4_PRED_DEC :        //stack[sp] = global[ stack[sp] ] -= param24;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_PRED_DEC" << VAL( vpu->mSp );)
        tmp = VAL( vpu->mSp );
        if( tmp > 0 && tmp < mMemorySize )
          MEM( vpu->mSp, mMemory[tmp] -= PARAM24 );
        else memFail( vpu, tmp );
        vpu->mIp += 4;
        break;


        //Переходы
      case VBC4_JUMP :            //ip = param
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_JUMP" << PARAM24;)
        vpu->mIp = PARAM24;
        break;

      case VBC4_TRUE_JUMP :       //if( stack[sp++] != 0 ) ip = param; Переход на метку res при условии
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_TRUE_JUMP" << VAL( vpu->mSp ) << PARAM24;)
        if( VAL(vpu->mSp++) ) vpu->mIp = PARAM24;
        else vpu->mIp += 4;
        break;

      case VBC4_TRUE_JUMP_KEEP :  //if( stack[sp] != 0 ) ip = param; else sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_TRUE_JUMP_KEEP" << VAL( vpu->mSp ) << PARAM24;)
        if( VAL(vpu->mSp) ) vpu->mIp = PARAM24;
        else {
          vpu->mSp++;
          vpu->mIp += 4;
          }
        break;

      case VBC4_FALSE_JUMP :      //if( stack[sp++] == 0 ) ip = param;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_FALSE_JUMP" << VAL( vpu->mSp ) << PARAM24;)
        if( !VAL(vpu->mSp++) ) vpu->mIp = PARAM24;
        else vpu->mIp += 4;
        break;

      case VBC4_FALSE_JUMP_KEEP : //if( stack[sp] == 0 ) ip = param; else sp++;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC4_FALSE_JUMP_KEEP" << VAL( vpu->mSp ) << PARAM24;)
        if( !VAL(vpu->mSp) ) vpu->mIp = PARAM24;
        else {
          vpu->mSp++;
          vpu->mIp += 4;
          }
        break;

      case VBC1_NOP :             //Холостая команда
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_NOP";)
        vpu->mIp++;
        break;

      //Обработка исключений
      //Объявить, что в данной функции останавливаются заданные исключения
      case VBC1_CATCH :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_CATCH" << VAL( vpu->mSp );)
        vpu->mTm = VAL(vpu->mSp++);
        vpu->mIp++;
        break;

      case VBC5_CATCH :           //tm = param;
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC5_CATCH" << PARAM32;)
        vpu->mTm = PARAM32;
        vpu->mIp += 5;
        break;

      //Возбудить исключение
      case VBC1_THROW :
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_THROW" << VAL( vpu->mSp );)
        doThrow( vpu, VAL(vpu->mSp++) );
        break;

      case VBC5_THROW :           //Делать:
                                  // Возврат из функции
                                  // Пока на корневая функция или маска исключения функции не совпадет с исключением
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC5_THROW" << PARAM32;)
        doThrow( vpu, PARAM32 );
        break;

        //Инструкции, порождаемые специальными функциями
      case VBC1_WAIT :            //Передача управления другому потоку
        SV_DEBUG(qDebug() << vpu->mIp << vpu->mSp << ":VBC1_WAIT";)
        vpu->mIp++;
        return;




      case VBC_LAST :
        break;
      }
    }
  }


  
  
void SvVMachine::executeHandler0( int addr ) {
  //Сохранить текущий адрес
  int ip = mVpuList[0].mIp;
  //mVpuList[0].mIp = 0; //Это чтобы исполнение остановилось в нужной точке
  //Сформировать вызов
  callInternal( mVpuList, addr, 0 );
  
  //Исполнить прерывание
  executeCore( mVpuList );
  
  //Вернуть текущий адрес
  mVpuList[0].mIp = ip;
  }
  
  
  
  
void SvVMachine::executeHandler1( int addr, int value0 ) {
  //Разместить параметр в стеке
  memSet( mVpuList, --(mVpuList[0].mSp), value0 );
  
  //Выполнить прерывание
  executeHandler0( addr );
  
  //Удалить параметр и стека
  mVpuList[0].mSp++;
  }
  
  
  
  
void SvVMachine::executeHandler2( int addr, int value0, int value1 ) {
  //Разместить параметр в стеке
  memSet( mVpuList, --(mVpuList[0].mSp), value0 );
  
  //Выполнить прерывание
  executeHandler1( addr, value1 );
  
  //Удалить параметр и стека
  mVpuList[0].mSp++;
  }
  
  
  
  



void SvVMachine::callInternal( SvVmVpu *vpu, int addr, int retAddr ) {
  //внутренние функции
  mMemory[vpu->mSp-1] = vpu->mBp;
  mMemory[vpu->mSp-2] = vpu->mTm;
  vpu->mTm = 0;
  mMemory[vpu->mSp-3] = retAddr;
  vpu->mSp -= 3;
  vpu->mBp = vpu->mSp;
  vpu->mThrow = 0;

  //Обычная функция
  vpu->mIp = addr;
  }




//Универсальный вызов подпрограммы
//по смещению addrOffset относительно sp содержится адрес вызова
bool SvVMachine::call(SvVmVpu *vpu, int addrOffset, int inc)
  {
  //Вызов любой функции
  //operand - это смещение комплексного адреса вызова
  //global[--sp] = bp;
  //global[--sp] = tm; tm = 0;
  //global[--sp] = ip; ip = addr;
  //bp = sp;
  //Комплексный адрес вызова содержит в одной 32-битной ячейке 3 поля
  // 31 - 1 - внешние функции
  //      0 - внутренние функции
  int tmp = VAL( vpu->mSp + addrOffset );
  if( vpu->mSp - 3 <= 0 || vpu->mSp > mMemorySize ) {
    memFail( vpu, vpu->mSp );
    vpu->mIp = 0;
    return true;
    }
  if( static_cast<unsigned>(tmp) & 0x80000000 ) {
    //import - функции
    //Статическая внешняя функция
    //Если вернется true, то переключаемся на другую задачу, а вызов функции будет повторен
    if( executeMethod( vpu, static_cast<int>( static_cast<unsigned>(tmp) & 0x7fffffff ) ) )
      return true;
    //Если вернется false, то выполнение программы будет продолжено
    vpu->mIp += inc;
    }
  else {
    vpu->mIp += inc;
    //внутренние функции
    callInternal( vpu, tmp, vpu->mIp );
    }
  //Не переключаемся и продолжаем выполнение
  return false;
  }




//Обработка исключения
void SvVMachine::doThrow(SvVmVpu *vpu, int mask)
  {
  if( mask )
    vpu->mThrow = mask;
  while( vpu->mBp != vpu->mBaseSp ) {
    vpu->mSp = vpu->mBp + 3;
    vpu->mIp = mMemory[vpu->mBp];
    vpu->mTm = mMemory[vpu->mBp+1];
    vpu->mBp = mMemory[vpu->mBp+2];
    if( vpu->mTm & vpu->mThrow )
      break;
    }
  }




