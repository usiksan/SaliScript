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

    Реализация класса SvOperatorReturn - оператор return
*/

#include "SvCompiler.h"

using namespace SvCompiler6;


//Сформировать листинг
SvOperatorReturn::SvOperatorReturn(const SvMark &src, const QString remark, SvFunction *fun) :
  SvOperator( src, remark ),
  mResult(nullptr),
  mFunction(fun)
  {
  }

SvOperatorReturn::~SvOperatorReturn()
  {
  if(mResult) delete mResult;
  }



QString SvOperatorReturn::listing(int level)
  {
  if( mResult ) return padding(level) + QString("return %1\n").arg(mResult->listing());
  return padding(level) + QString("return\n");
  }


