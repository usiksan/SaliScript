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

    Реализация класса SvFunctionType - тип, описывающий функцию
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvFunctionType::~SvFunctionType() {
  }



QString SvFunctionType::buildSignature()
  {
  mSignature = QString("<%1(").arg( mResult->mSignature );
  //Пройти по параметрам
  for( SvFunParam &param : mParamList ) {
    mSignature.append(",").append(param.mType->mSignature);
    }
  //Завершение
  mSignature.append(")>");
  return mSignature;
  }





