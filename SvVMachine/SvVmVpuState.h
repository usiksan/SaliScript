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
    Состояние виртуального процессора
*/
#ifndef SVVMVPUSTATE_H
#define SVVMVPUSTATE_H

struct SvVmVpuState {
    int   mIp;              //!< instruction pointer [указатель инструкций]
    int   mSp;              //!< stack pointer [указатель стека]
    int   mBp;              //!< function frame pointer [указатель базы в стеке локальных переменных для текущей функции (указывает на фрейм возврата из функции)]
    int   mTm;              //!< exception mask [маска обрабатываемых исключений]
    int   mBaseSp;          //!< stack start [Начало стека для данного процессора]
    int   mThrow;           //!< current exception [Текущее значение исключения]
    int   mDebugRun;        //!< if eq 0 then in debug state else - in run state

    SvVmVpuState() : mIp(0), mSp(0), mBp(0), mTm(0), mBaseSp(0), mThrow(0), mDebugRun(0) {}
  };

#endif // SVVMVPUSTATE_H
