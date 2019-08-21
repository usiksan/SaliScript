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

    Реализация класса SvValue - базовое значение
*/


#include "SvCompiler.h"

using namespace SvCompiler6;

SvValue::SvValue(SvType *type, const SvMark &mark) :
  mType(type),
  mMark( mark ),
  mConstInt(0),
  mConst(false)
  {

  }



