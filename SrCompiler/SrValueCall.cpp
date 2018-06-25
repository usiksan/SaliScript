/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvValueCall - call function
*/

#include "SrCompiler.h"

using namespace SrCompiler6;


SrValueCall::SrValueCall(SrValue *fun, SrType *result, const SrMark &mark) :
  SrValue( result, mark ),
  mFunction(fun),
  mParamCount(0)
  {
  memset( mParam, 0, sizeof(mParam) );
  }



SrValueCall::~SrValueCall() {
  for( int i = 0; i < mParamCount; i++ )
    if( mParam[i] ) delete mParam[i];
  }



SrValuePtr &SrValueCall::nextParam()
  {
  if( mParamCount < SV_FUN_PARAM_MAX - 1 )
    return mParam[mParamCount++];
  if( mParam[mParamCount] ) {
    delete mParam[mParamCount];
    mParam[mParamCount] = 0;
    }
  return mParam[mParamCount];
  }




//Получить строку листинга
QString SrValueCall::listing()
  {
  return QString("call %1").arg( mFunction->listing() );
  }









