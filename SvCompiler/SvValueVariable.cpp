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

    Реализация класса SvValueVariable - переменная
*/


#include "SvCompiler.h"

using namespace SvCompiler6;

SvValueVariable::SvValueVariable(SvVariable *var, const SvMark &mark ) :
  SvValue(nullptr, mark ),
  mVariable(var),
  mAddonAddress(0)
  {
  if( mVariable->mType->isArray() )
    //Для массивов по любому адрес
    mType = mVariable->mType->mBaseType->getTypePointer();
  else
    //иначе просто тип
    mType = mVariable->mType;
  }






