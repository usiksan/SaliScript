/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvOperatorBlock - block of operators
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrOperatorBlock::SrOperatorBlock(const SrMark &mark, const QString remark, SrOperatorContext *parent) :
  SrOperatorContext( mark, remark, parent )
  {

  }

SrOperatorBlock::~SrOperatorBlock()
  {
  qDeleteAll(mList);
  }

QString SrOperatorBlock::listing(int level)
  {
  level++;
  //Пролог
  QString str( padding(level) );
  str.append("{\n");
  //Переменные блока
  str.append( mVarLocal.listing(level) );
  for( SrOperator *op : mList )
    str.append( op->listing(level) );
  //Эпилог
  str.append( padding(level) ).append("}\n");
  return str;
  }
