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

    Класс SvSource
*/
#include "SvCompiler/SvCompiler.h"

using namespace SvCompiler6;



SvSource::~SvSource() {
  if( mInputStream ) delete mInputStream;
  }



//Прочитать одну строку из источника
QString
SvSource::ReadLine() {
  if( mInputStream ) {
    mLineCount++;
    return mInputStream->readLine();
    }
  return QString();
  }


//Определить, дошли ли до конца потока
bool
SvSource::Eof() {
  if( mInputStream )
    return mInputStream->atEnd();
  return true;
  }
