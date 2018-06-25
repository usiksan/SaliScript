/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvValue - basic value
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrValue::SrValue(SrType *type, const SrMark &mark) :
  mType(type),
  mMark( mark ),
  mConstInt(0),
  mConst(false)
  {

  }



