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
    Виртуальный процессор виртуальной машины
*/
#ifndef SVVMVPU
#define SVVMVPU

#include "SvVMachine/SvVmVpuState.h"

//Virtual processing unit [Виртуальный процессор]
struct SvVmVpu : public SvVmVpuState {
    int   mDebugRunWhile;
    int   mDebugBreakStart; //
    int   mDebugBreakStop;
    int   mDebugBp;

    //Инициализировать для заданной точки входа
    void enter( int start, int stack ) {
      mIp = start;
      mBp = mSp = mBaseSp = stack;
      mTm = mThrow = 0;
      mDebugRun = true;
      }

    //Исполнять без остановки
    void  debugRun() {
      mDebugRun = true;
      }

    //Исполнять, пока находится в диапазоне и bp меньше debugBp (trace)
    void  debugRunTrace( int start, int stop ) {
      mDebugBreakStart = start;
      mDebugBreakStop = stop;
      mDebugRunWhile = true;
      mDebugBp = 0;
      mDebugRun = false;
      }

    //Исполнять, пока находится в диапазоне или bp меньше debugBp (step)
    void  debugRunStep( int start, int stop ) {
      mDebugBreakStart = start;
      mDebugBreakStop = stop;
      mDebugRunWhile = true;
      mDebugBp = mBp;
      mDebugRun = false;
      }

    //Исполнять, пока находится вне диапазона (break point)
    void debugRunUntil( int start, int stop ) {
      mDebugBreakStart = start;
      mDebugBreakStop = stop;
      mDebugRunWhile = false;
      mDebugRun = false;
      }

    //Исполняется или нет
    bool isRun() const {
      if( !mDebugRun ) {
        if( mDebugRunWhile ) {
          //Выполнять, пока адреса находятся в заданном диапазоне или bp меньше debugBp
          return (mIp >= mDebugBreakStart && mIp <= mDebugBreakStop) || mBp < mDebugBp;
//          if( mIp < mDebugBreakStart || mIp > mDebugBreakStop )
//            return false;
          }
        else {
          //Выполнять, пока адреса не очутятся в заданном диапазоне
          if( mIp >= mDebugBreakStart && mIp <= mDebugBreakStop )
            return false;
          }
        }
      return true;
      }
  };




#endif // SVVMVPU

