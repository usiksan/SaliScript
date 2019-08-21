/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Пико-компилятор"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Проверка виртуальной машины
*/
#include "SvVMachine/SvVMachine.h"
#include <QCoreApplication>
#include <QDebug>

class Machine : public SvVMachine {
  public:

    // SvVMachine interface
  public:
    virtual void memFail(SvVmVpu *vpu, int index) override;
    virtual void progFail(SvVmVpu *vpu, int address, int len) override;
  };

void Machine::memFail(SvVmVpu *vpu, int index)
{
}


void Machine::progFail(SvVmVpu *vpu, int address, int len)
{
}

int main(int argc, char *argv[])
  {
  QCoreApplication a(argc, argv);
  Machine machine;

  qDebug() << sizeof(int);
  return a.exec();
  }
