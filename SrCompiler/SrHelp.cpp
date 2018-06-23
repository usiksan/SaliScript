/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvHelp - help marker system
*/

#include "SrCompiler.h"

using namespace SrCompiler6;


SrHelp::SrHelp() :
  mBrief(),
  mReference(),
  mDefinePos()
  {

  }

SrHelp::SrHelp(const SrHelp &hlp) :
  mBrief(hlp.mBrief),
  mReference(hlp.mReference),
  mDefinePos(hlp.mDefinePos)
  {

  }

void SrHelp::update(const SrHelp &hlp)
  {
  if( mBrief.isEmpty() ) mBrief = hlp.mBrief;
  if( mReference.isEmpty() ) mReference = hlp.mReference;
  }

void SrHelp::clear()
  {
  mBrief.clear();
  mReference.clear();
  mDefinePos.clear();
  }

