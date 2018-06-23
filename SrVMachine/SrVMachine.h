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

#ifndef SVCONTROLLER_H
#define SVCONTROLLER_H

#include "SrVMachine/SrVmTypes.h"
#include "SrVMachine/SrVmCodeHeader.h"
#include "SrVMachine/SrVmVpu.h"
#include "SrVMachine/SrVmUtils.h"

/*!
   \brief The SrVMachine class - виртуальная машина для исполнения байт-кода

    Виртуальная машина содержит в себе память программ, память данных
    и набор виртуальных процессоров. Класс SrVMachine не может использоваться напрямую.
    Вместо этого должен быть создан производный класс в котором будут реализованы
    функции реакции на нарушение доступа к памяти, а также функция executeMethod,
    в которой определяются внешние, по отношению к скрипту, функции.

    Также важно помнить, что конструктор SrVMachine по умолчанию не выделяет никакой памяти.
    Поэтому количество памяти программ, данных и количество виртуальных процессоров - все это
    задается в производном классе.

 */
class SrVMachine
  {
  protected:
    int            *mMemory;          //! Указатель на память данных
    int             mMemorySize;      //! Размер памяти данных
    int             mStack;           //! Положение стека для операций выделения стека для процессов
    SrVmCode       *mProg;            //! Указатель на программу (память программ)
    int             mProgSize;        //! Размер памяти программ
    SrVmVpu        *mVpuList;         //! Список виртуальных процессоров
    int             mVpuCount;        //! Количество активных виртуальных процессоров
    int             mVpuMax;          //! Максимальное количество виртуальных процессоров
  public:

    /*!
       \brief SrVMachine Конструктор виртуальной машины.
     */
    SrVMachine();
    virtual ~SrVMachine();

    //Информационные
    /*!
       \brief getMemorySize Возвращает размер памяти данных
       \return Размер памяти данных
     */
    int getMemorySize() const { return mMemorySize; }

    /*!
       \brief getVpuMax Возвращает максимальное количество виртуальных процессоров
       \return Максимальное количество виртуальных процессоров
     */
    int getVpuMax() const { return mVpuMax; }


    /*!
       \brief processing Выполняет один шаг обработки всех активных виртуальных процессоров
       \param timeOffset - интервал времени, который нужно прибавить к счетчику системного времени (в мс)

       Эта функция выполняет один цикл обработки всех активных виртуальных процессоров.

       Перед началом этого цикла к встроенной переменной, которая отвечает за отсчет временных
       интервалов vpuTickCount добавляет смещение timeOffset. Время, записанное в этой переменной
       показывает количество милисекунд с момента старта виртуальной машины. При длительной работе
       виртуальной машины существует проблема заворачивания значения времени. Эта проблема легко
       решается путем правильного вычисления временных интервалов (timeOut - vpuTickCount) > 0.
       Приведенным образом можно отслеживать интервалы продолжительностью до 23 дней, что на практике
       более чем достаточно. Для работы со временем также можно использовать календарь.

       Активным считается виртуальный процессор, который не находится в состоянии останова.

       Обработка одного активного виртуального процессора включает в себя последовательное исполнение
       команд начиная с текущей до момента вызова функции переключения задач. Для исключения
       зацикливания при отладке скрипта установлено ограничение на количество команд в последовательности,
       после которого все-равно произойдет переключение задачи.
     */
    virtual void processing( int timeOffset );


    /*!
       \brief memFail Вызывается из виртуальной машины при ошибке доступа к памяти
         данных. В производных классах должна быть реализована соответствующая реакция
         Такая ошибка возникает при попытке обратиться к несуществующей памяти.
       \param vpu Виртуальный процессор, вызвавший нарушение
       \param index Индекс ячейки памяти, который вызвал нарушение
     */
    virtual void memFail( SrVmVpu *vpu, int index ) = 0;


    /*!
       \brief progFail Вызывается из виртуальной машины при ошибке доступа к памяти
         программ. В производных класса должна быть реализована соответствующая реакция.
         Данная ошибка возникает при попытке обратиться по несуществующему адресу
         памяти программ.
       \param vpu Виртуальный процессор, вызвавший нарешние
       \param address Начальный адрес, по которому производилось считывание
       \param len Длина данных, подлежащих считыванию
     */
    virtual void progFail( SrVmVpu *vpu, int address, int len ) = 0;


    /*!
       \brief executeMethod Выполняет внешнюю, по отношению к скрипту, функцию
       \param vpu Виртуальный процессор, вызвавший внешнюю функцию
       \param methodId Числовой идентификатор внешней функции
       \return Возвращает истину, если нужно ждать (переключиться на другую задачу).
         Если вернуть ложь, то выполнение программы будет продолжено.

       Данная функция реализует механизм вызова из скрипта функций обработки - драйверов.
       Такие функции могут выполнить обращение к оборудованию и/или
       выполнить сложные ресурсоемкие вычисления.

       Для определения внешний функций в скрипте используется следующий механизм:
       \code
       file.h
       #define MY_FUNC_ID 1 //Числовой идентифкатор внешней функции
       ...
       #ifdef VPU_COMPILER
       //Описание прототипа внешней функции
       import MY_FUNC_ID void myFunc( int a, int b );
       #endif
       \endcode

       Файл file.h может включаться в программу скрипта, чтобы правильно вызывать внешние функции.
       Также он может включаться в файл конкретной реализации виртуальной машины, где в
       переопределении executeMethod будет реализована эта функция.
     */
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
    int  constBlock( int index ) { return svIRead24( mProg + constTable() + index * 4 ); }

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

            //Обработка исключения
            void doThrow( SrVmVpu *vpu, int mask );

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
