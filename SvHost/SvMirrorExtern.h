/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Host-система"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Базовое зеркало внешних VPU. Содержит отображение памяти и процессов.
*/

#ifndef SVMIRROREXTERN_H
#define SVMIRROREXTERN_H

#include "SvMirror.h"
#include "SvVMachine/SvVmVpuState.h"
#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>


//Задание отладки для задачи
struct SvDebugTask {
    int mCommand; //Команда отладки
    int mParam1;  //Параметр 1 для команды отладки
    int mParam2;  //Параметр 2 для команды отладки

    void set( int cmd, int p1 = 0, int p2 = 0 ) { mCommand = cmd; mParam1 = p1; mParam2 = p2; }
  };

class SvMirrorExtern : public SvMirror
  {
    Q_OBJECT

  protected:
    SvVmVpuState  *mVpuState;    //Зеркало состояния виртуальных машин
    SvDebugTask   *mVpuDebug;    //Команды отладочного управления
    QMutex         mVpuMutex;    //Механизм защиты от сдвоенного доступа к командам управления
    int            mVpuCount;    //Количество работающих VPU
    int            mVpuMax;      //Максимальное количество VPU
    int           *mMemory;      //Зеркало памяти
    int            mMemorySize;  //Размер памяти данных
    int            mMemoryCount; //Размер памяти глобальных переменных

    //очередь переменных на запись в контроллер
    QList<int>     mWriteQueue;  //Индексы переменных, значения которых нужно записать
    QMap<int, int> mWriteValues; //Перечень записываемых значений индекс(ключ)-значение
    QMutex         mWriteMutex;  //Механизм защиты от сдвоенного доступа

  public:
    SvMirrorExtern( bool scanTasks );
    virtual ~SvMirrorExtern() override;

    //===========================
    //Раздел списка задач
    virtual int           taskCount() const override { return mVpuCount; }
    virtual int           taskMax() const override { return mVpuMax; }

    //Получить информацию по задаче
    virtual SvVmVpuState *taskInfo( qint32 taskId ) override;

    //===========================
    //Раздел памяти данных

    virtual int           memorySize() const override { return mMemorySize; }

    //Получить состояние ячейки памяти
    virtual int           memoryGet( int index ) override;

    //Установить состояние ячейки памяти
    virtual void          memorySet( int index, int value ) override;

    //===========================
    //Раздел управления отладкой

    //Отладка - пуск
    virtual void          debugRun( int taskId ) override;

    //Отладка - исполнять пока внутри и не изменится bp (шаг)
    virtual void          debugRunStep( int taskId, int start, int stop ) override;

    //Отладка - исполнять пока снаружи (точка останова)
    virtual void          debugRunUntil( int taskId, int start, int stop ) override;

    //Отладка - исполнять пока внутри (трассировка)
    virtual void          debugRunTrace( int taskId, int start, int stop ) override;

  protected:
    //Построить зеркало в соответствии с удаленным контроллером
    void setupMirror( int vpuMax, int vpuCount, int memoryMax, int memoryCount );

  };

#endif // SVMIRROREXTERN_H
