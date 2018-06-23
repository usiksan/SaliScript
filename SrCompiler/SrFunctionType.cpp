/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvFunctionType - function type
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrFunctionType::~SrFunctionType() {
  }



QString SrFunctionType::buildSignature()
  {
  mSignature = QString("<%1(").arg( mResult->mSignature );
  //Пройти по параметрам
  for( SrFunParam &param : mParamList ) {
    mSignature.append(",").append(param.mType->mSignature);
    }
  //Завершение
  mSignature.append(")>");
  return mSignature;
  }





