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

    Класс SrSourceFile
*/
#include "SrCompiler/SrCompiler.h"

using namespace SrCompiler6;

SrSourceFile::SrSourceFile(QFile *file, int fileId) :
  SrSource( fileId ),
  mFile(file)
  {
  mInputStream = new QTextStream(mFile);
  }

SrSourceFile::~SrSourceFile() {
  if( mFile ) delete mFile;
  }

