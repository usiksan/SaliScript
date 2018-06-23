/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SrType - base type
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrValueBinaryLong::SrValueBinaryLong(SrValue *oper1, SrType *type, const SrMark &mark ) :
  SrValue( mark ),
  mOperCount(0)
  {
  mType = type;
  memset( mOperand, 0, sizeof(mOperand) );
  mOperand[0] = oper1;
  if( oper1 ) mOperCount = 1;
  }


SrValueBinaryLong::~SrValueBinaryLong() {
  for( int i = 0; i < mOperCount; i++ )
    if( mOperand[i] ) delete mOperand[i];
  }



SrValuePtr &SrValueBinaryLong::nextOperand()
  {
  if( mOperCount < SV_OPER_MAX - 1 )
    return mOperand[mOperCount++];
  if( mOperand[mOperCount] ) {
    delete mOperand[mOperCount];
    mOperand[mOperCount] = 0;
    }
  return mOperand[mOperCount];
  }




//Получить строку листинга
QString SrValueBinaryLong::listing()
  {
  QString str;
  if( mOperand[0] ) {
    str.append("(").append( mOperand[0]->listing() ).append(")");
    for( int i = 1; i < mOperCount; i++ )
      if( mOperand[i] )
        str.append( operation() ).append("(").append(mOperand[i]->listing()).append(")");
    }
  else str = QString("ERR");
  return str;
  }

