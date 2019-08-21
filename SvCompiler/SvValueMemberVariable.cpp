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

    Реализация класса SvValueMemberVariable - переменная-член структуры
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvValueMemberVariable::SvValueMemberVariable(SvVariable *var, SvValue *struc, const SvMark &mark) :
  SvValueVariable( var, mark ),
  mStruct( struc )
  {

  }

SvValueMemberVariable::~SvValueMemberVariable()
  {
  if( mStruct )
    delete mStruct;
  }


