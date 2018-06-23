/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "SrVMachine/SrVMachine.h"
#include <QCoreApplication>
#include <QDebug>

class Machine : public SrVMachine {
  public:

    // SrVMachine interface
  public:
    virtual void memFail(SrVmVpu *vpu, int index) override;
    virtual void objFail(SrVmVpu *vpu, int index) override;
    virtual void progFail(SrVmVpu *vpu, int address, int len) override;
  };

void Machine::memFail(SrVmVpu *vpu, int index)
{
}

void Machine::objFail(SrVmVpu *vpu, int index)
{
}

void Machine::progFail(SrVmVpu *vpu, int address, int len)
{
}

int main(int argc, char *argv[])
  {
  QCoreApplication a(argc, argv);
  Machine machine;

  qDebug() << sizeof(int);
  return a.exec();
  }
