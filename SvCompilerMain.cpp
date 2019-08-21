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
    Проверка компилятора на тестовых скриптах
*/

#include "SvCompiler/SvCompiler.h"
#include "SvCompiler/SvVpuCompiler.h"

#include <QCoreApplication>
#include <QDebug>


int main(int argc, char *argv[])
  {
  QCoreApplication a(argc, argv);
  SvCompiler6::SvVpuCompiler compiler;
  compiler.make( QString("d:/"), QString("test1.c") );

  return 0;
  }
