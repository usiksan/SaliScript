/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvTypeList - type list
*/

#include "SrCompiler.h"

using namespace SrCompiler6;


//Сформировать листинг
SrOperatorReturn::SrOperatorReturn(const SrMark &src, const QString remark, SrFunction *fun) :
  SrOperator( src, remark ),
  mResult(0),
  mFunction(fun)
  {
  }

SrOperatorReturn::~SrOperatorReturn()
  {
  if(mResult) delete mResult;
  }



QString SrOperatorReturn::listing(int level)
  {
  if( mResult ) return padding(level) + QString("return %1\n").arg(mResult->listing());
  return padding(level) + QString("return\n");
  }


