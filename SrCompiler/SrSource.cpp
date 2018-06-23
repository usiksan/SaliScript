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

    Класс SrSource
*/
#include "SrCompiler/SrCompiler.h"

using namespace SrCompiler6;



SrSource::~SrSource() {
  if( mInputStream ) delete mInputStream;
  }



//Прочитать одну строку из источника
QString
SrSource::ReadLine() {
  if( mInputStream ) {
    mLineCount++;
    return mInputStream->readLine();
    }
  return QString();
  }


//Определить, дошли ли до конца потока
bool
SrSource::Eof() {
  if( mInputStream )
    return mInputStream->atEnd();
  return true;
  }
