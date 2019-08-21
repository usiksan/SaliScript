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

    Реализация класса SvValueCall - вызов функции
*/

#include "SvCompiler.h"

using namespace SvCompiler6;


SvValueCall::SvValueCall(SvValue *fun, SvType *result, const SvMark &mark) :
  SvValue( result, mark ),
  mFunction(fun),
  mParamCount(0)
  {
  memset( mParam, 0, sizeof(mParam) );
  }



SvValueCall::~SvValueCall() {
  for( int i = 0; i < mParamCount; i++ )
    if( mParam[i] ) delete mParam[i];
  }



SvValuePtr &SvValueCall::nextParam()
  {
  if( mParamCount < SV_FUN_PARAM_MAX - 1 )
    return mParam[mParamCount++];
  if( mParam[mParamCount] ) {
    delete mParam[mParamCount];
    mParam[mParamCount] = nullptr;
    }
  return mParam[mParamCount];
  }




//Получить строку листинга
QString SvValueCall::listing()
  {
  return QString("call %1").arg( mFunction->listing() );
  }









