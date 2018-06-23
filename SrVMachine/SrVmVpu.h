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
    Virtual machine processor
*/

#ifndef SRVMVPU
#define SRVMVPU

#include "SrVMachine/SrVmVpuState.h"

//Virtual processing unit [Виртуальный процессор]
struct SrVmVpu : public SrVmVpuState {
    int   mDebugRunWhile;
    int   mDebugBreakStart; //
    int   mDebugBreakStop;
    int   mDebugBp;

    //Инициализировать для заданной точки входа
    void enter( int start, int stack ) {
      mIp = start;
      mBp = mSp = mBaseSp = stack;
      mTm = mThrow = 0;
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

    //Исполнять, пока находится в диапазоне и bp меньше debugBp (step)
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
          //Выполнять, пока адреса находятся в заданном диапазоне
          if( mIp < mDebugBreakStart || mIp > mDebugBreakStop )
            return false;
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




#endif // SRVMVPU

