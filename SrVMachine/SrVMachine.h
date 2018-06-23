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
    Virtual machine for programm execution
    One machine contains some VPUs
*/

#ifndef SVCONTROLLER_H
#define SVCONTROLLER_H

#include "SrVMachine/SrVmTypes.h"
#include "SrVMachine/SrVmCodeHeader.h"
#include "SrVMachine/SrVmVpu.h"
#include "SrVMachine/SrVmUtils.h"

//#define NO_FLOAT

class SrVMachine
  {
  protected:
    int            *mMemory;          //Указатель на память данных
    int             mMemorySize;      //Размер памяти данных
    int             mStack;           //Положение стека для операций выделения стека для процессов
    SrVmCode       *mProg;            //Указатель на программу (память программ)
    int             mProgSize;        //Размер памяти программ
    SrVmVpu        *mVpuList;         //Список виртуальных процессоров
    int             mVpuCount;        //Количество активных виртуальных процессоров
    int             mVpuMax;          //Максимальное количество виртуальных процессоров
  public:
    SrVMachine();
    virtual ~SrVMachine();

    //Информационные
    int getMemorySize() const { return mMemorySize; }
    int getVpuMax() const { return mVpuMax; }

    //Обработка всех активных виртуальных процессоров
    //timeOffset - интервал времени, который нужно прибавить к счетчику системного времени (в мс)
    virtual void processing( int timeOffset );

    //Нарушение доступа к памяти данных
    virtual void memFail( SrVmVpu *vpu, int index ) = 0;

    //Нарушение доступа к объектам
    virtual void objFail( SrVmVpu *vpu, int index ) = 0;

    //Нарушение доступа к памяти программ
    virtual void progFail( SrVmVpu *vpu, int address, int len ) = 0;

    //Выполнить внешний метод
    //Возвращает истину, если нужно ждать (переключиться на другую задачу)
    virtual bool executeMethod( SrVmVpu *vpu, int methodId );


    //===========================
    //Раздел исполнения прерываний
    void executeHandler0( int addr );
    
    void executeHandler1( int addr, int value0 );
    
    void executeHandler2( int addr, int value0, int value1 );



    //===========================
    //Раздел управления

    //Установить новую программу
    void            setProgramm( const SrVmCode *code);

    //Все очистить и убрать все виртуальные процессоры
    void            reset();

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    void            restart( bool run );

    //Количество активных задач
    int             taskCount() const { return mVpuCount; }

    const SrVmVpu  *vpu( int id ) const { return mVpuList + id; }

    int             vpuId( SrVmVpu *vpu ) { return vpu - mVpuList; }

    const SrVmCode *code() const { return mProg; }


    //===========================
    //Information from programm [Информация из программы]
    //Programm hash [Хэш значение программы]
    int  hashProg()    const { return svIRead32( mProg + SRVMH_HASH ); }
    //Programm name [Сигнатура (имя программы) Текстовое поле длиной 32 байт]
    int  signature0()  const { return svIRead32( mProg + SRVMH_SIGNATURE ); }
    int  signature1()  const { return svIRead32( mProg + SRVMH_SIGNATURE + 4 ); }
    int  signature2()  const { return svIRead32( mProg + SRVMH_SIGNATURE + 8 ); }
    int  signature3()  const { return svIRead32( mProg + SRVMH_SIGNATURE + 12 ); }
    int  signature4()  const { return svIRead32( mProg + SRVMH_SIGNATURE + 16 ); }
    //Таблица инициализации
    //Каждая запись таблицы адрес 3 байта : значение 4 байта
    //Последняя запись - нуль
    int  initTable()   const { return svIRead32( mProg + SRVMH_INIT_TABLE ); }
    //Точка входа
    int  enterPoint()  const { return svIRead32( mProg + SRVMH_ENTER_POINT ); }
    //Размер стека для начальной задачи
    int  stackSize0()  const { return svIRead32( mProg + SRVMH_STACK_SIZE0 ); }
    //Размер программы VPU (только код)
    int  vpuProgSize() const { return svIRead32( mProg + SRVMH_VM_PROG_SIZE ); }
    //Количество глобальных переменных
    int  globalCount() const { return svIRead32( mProg + SRVMH_GLOBAL_COUNT ); }
    //Таблица строк
    int  constTable() const { return svIRead32( mProg + SRVMH_CONST_TABLE ); }
    //Размер всей программы
    int  progSize()    const { return svIRead32( mProg + SRVMH_PROG_SIZE ); }
    //Const block address
    int  constBlock( int index ) { return svIRead24( constTable() + index * 4 ); }

    //===========================
    //Раздел управления отладкой

    //Отладка - пуск
    void debugRun( int vpu );

    //Отладка - стоп (пауза)
    void debugPause( int vpu );

    //Исполнять, пока находится в диапазоне и bp меньше debugBp (trace)
    void debugRunTrace( int vpu, int start, int stop );

    //Исполнять, пока находится в диапазоне и bp меньше debugBp (step)
    void debugRunStep( int vpu, int start, int stop );

    //Исполнять, пока находится вне диапазона (break point)
    void debugRunUntil( int vpu, int start, int stop );


    virtual void memSet( SrVmVpu *vpu, int index, int value ) {
      if( index > 0 && index < mMemorySize )
        mMemory[index] = value;
      else memFail( vpu, index );
      }

    virtual int memGet( SrVmVpu *vpu, int index ) {
      if( index > 0 && index < mMemorySize )
        return mMemory[index];
      memFail( vpu, index );
      return 0;
      }


  protected:
    //Вычислительное ядро
            void executeCore( SrVmVpu *vpu );

            void callInternal( SrVmVpu *vpu, int addr, int retAddr );

            //Универсальный вызов подпрограммы
            //по смещению addrOffset относительно sp содержится адрес вызова
            void call( SrVmVpu *vpu, int addrOffset );

            //1 byte param at index address
            int value1( SrVmVpu *vpu, int index ) {
              if( index < 0 || index >= mProgSize ) {
                progFail( vpu, index, 1 );
                return 0;
                }
              return (signed char)(mProg[index]);
              }

            //2 byte param at index address
            int value2( SrVmVpu *vpu, int index ) {
              if( index < 0 || index + 1 >= mProgSize ) {
                progFail( vpu, index, 2 );
                return 0;
                }
              return svIRead16( mProg + index );
              }

            //3 byte param at index address
            int value3( SrVmVpu *vpu, int index ) {
              if( index < 0 || index + 2 >= mProgSize ) {
                progFail( vpu, index, 3 );
                return 0;
                }
              return svIRead24( mProg + index );
              }

            //4 byte param at index address
            int value4( SrVmVpu *vpu, int index ) {
              if( index < 0 || index + 3 >= mProgSize ) {
                progFail( vpu, index, 4 );
                return 0;
                }
              return svIRead32( mProg + index );
              }
  };

//Макросы для упрощенной записи доступа к параметрам внешних методов и их результатам
#define VPU_GET_FUN_PARAM( idx ) memGet( vpu, vpu->mSp + (idx) )
#define VPU_SET_FUN_RESULT( idx, val ) memSet( vpu, vpu->mSp + (idx), val )
//Пример реализации
//1             0
//int  Abs( int val );
//VPU_SET_FUN_RESULT( 1, abs(VPU_GET_FUN_PARAM(0)) );


#endif // SVCONTROLLER_H
