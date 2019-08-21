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

    Реализация класса SvValueBinaryLong - бинарная повторяющаяся операция (n-нарная)
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvValueBinaryLong::SvValueBinaryLong(SvValue *oper1, SvType *intType, const SvMark &mark ) :
  SvValue( intType, mark ),
  mOperCount(0)
  {
  memset( mOperand, 0, sizeof(mOperand) );
  mOperand[0] = oper1;
  if( oper1 ) mOperCount = 1;
  }


SvValueBinaryLong::~SvValueBinaryLong() {
  for( int i = 0; i < mOperCount; i++ )
    if( mOperand[i] ) delete mOperand[i];
  }



SvValuePtr &SvValueBinaryLong::nextOperand()
  {
  if( mOperCount < SV_OPER_MAX - 1 )
    return mOperand[mOperCount++];
  if( mOperand[mOperCount] ) {
    delete mOperand[mOperCount];
    mOperand[mOperCount] = nullptr;
    }
  return mOperand[mOperCount];
  }




//Получить строку листинга
QString SvValueBinaryLong::listing()
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

