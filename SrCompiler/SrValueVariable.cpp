/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvValueVariable - variable
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrValueVariable::SrValueVariable(SrVariable *var, const SrMark &mark ) :
  SrValue(nullptr, mark ),
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






