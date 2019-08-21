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
    Пико компилятор скриптового языка rc++

    Класс SvSourceFile
*/
#include "SvCompiler/SvCompiler.h"

using namespace SvCompiler6;

SvSourceFile::SvSourceFile(QFile *file, int fileId) :
  SvSource( fileId ),
  mFile(file)
  {
  mInputStream = new QTextStream(mFile);
  }

SvSourceFile::~SvSourceFile() {
  if( mFile ) delete mFile;
  }

