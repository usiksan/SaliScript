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

    Реализация класса SvHelp - маркер системы помощи
*/

#include "SvCompiler.h"

using namespace SvCompiler6;


SvHelp::SvHelp() :
  mBrief(),
  mReference(),
  mDefinePos()
  {

  }

SvHelp::SvHelp(const SvHelp &hlp) :
  mBrief(hlp.mBrief),
  mReference(hlp.mReference),
  mDefinePos(hlp.mDefinePos)
  {

  }

void SvHelp::update(const SvHelp &hlp)
  {
  if( mBrief.isEmpty() ) mBrief = hlp.mBrief;
  if( mReference.isEmpty() ) mReference = hlp.mReference;
  }

void SvHelp::clear()
  {
  mBrief.clear();
  mReference.clear();
  mDefinePos.clear();
  }

