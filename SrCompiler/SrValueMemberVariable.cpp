/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvValueMemberVariable - value of member variable
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

SrValueMemberVariable::SrValueMemberVariable(SrVariable *var, SrValue *struc, const SrMark &mark) :
  SrValueVariable( var, mark ),
  mStruct( struc )
  {

  }

SrValueMemberVariable::~SrValueMemberVariable()
  {
  if( mStruct )
    delete mStruct;
  }


