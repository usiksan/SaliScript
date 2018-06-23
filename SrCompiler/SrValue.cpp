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

SrValue::SrValue(const SrMark &mark) :
  mType(0),
  mMark( mark ),
  mConstInt(0),
  mConst(false)
  {

  }



