#include "SrCompiler/SrCompiler.h"
#include "SrCompiler/SrVpuCompiler.h"

#include <QCoreApplication>
#include <QDebug>


int main(int argc, char *argv[])
  {
  QCoreApplication a(argc, argv);
  SrCompiler6::SrVpuCompiler compiler;
  compiler.make( QString("/home/salilab/"), QString("test.c") );

  return 0;
  }
