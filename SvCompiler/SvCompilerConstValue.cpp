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

    Реализация класса TCompiler часть A (вычисление константных выражений)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;

void SvCompiler::constValueCalc( SvValue *val )
  {
  if( val == nullptr ) return;
  //val->mType = mTypeInt;
  switch( val->code() ) {

    case svvVariable :       //Переменная (глобальная, локальная, параметр)
    case svvFunction :       //Функция (глобальная)
    case svvWaitFun :        //Специальная функция svWait
    case svvThrowFun :       //Специальная функция svThrow
    case svvCatchFun :       //Специальная функция svCatch
    case svvExceptionFun :   //Специальная функция svException
    case svvMemberVariable : //Член от произвольной структуры структуры
    case svvConstString :
    case svvPointer :
    case svvAddress :
    case svvPredInc :
    case svvPredDec :
    case svvPostInc :
    case svvPostDec :
    case svvStore :
    case svvArrayCell :      //Операция индекса массива
    case svvMulStore :
    case svvDivStore :
    case svvModStore :
    case svvAddStore :
    case svvSubStore :
    case svvLShiftStore :
    case svvRShiftStore :
    case svvOrStore :
    case svvAndStore :
    case svvXorStore :
    case svvLogAnd :
    case svvLogOr :
    case svvComma :      //Операция ,
    case svvCondition :      //Тройная условная операция
    case svvCall :      //Вызов функции
    case svvLast :
    case svvConstInt :      //Константа
      break;

    case svvNot :
      constValueCalc( dynamic_cast<SvValueBitNot*>(val)->mOperand );
      dynamic_cast<SvValueBitNot*>(val)->checkConst();
      break;

    case svvLogNot :
      constValueCalc( dynamic_cast<SvValueLogNot*>(val)->mOperand );
      dynamic_cast<SvValueLogNot*>(val)->checkConst();
      break;

    case svvNeg :
      constValueCalc( dynamic_cast<SvValueNeg*>(val)->mOperand );
      dynamic_cast<SvValueNeg*>(val)->checkConst();
      break;

    case svvMul :
    case svvDiv :
    case svvMod :
    case svvAdd :
    case svvSub :
    case svvLShift :
    case svvRShift :
    case svvEqu :
    case svvNotEqu :
    case svvLessEqu :
    case svvLess :
    case svvGreat :
    case svvGreatEqu :
      constValueCalc( dynamic_cast<SvValueBinary*>(val)->mOperand1 );
      constValueCalc( dynamic_cast<SvValueBinary*>(val)->mOperand2 );
      dynamic_cast<SvValueBinary*>(val)->checkConst();
      break;

    case svvOr :
    case svvAnd :
    case svvXor :
      constValueLongCalc( dynamic_cast<SvValueBinaryLong*>(val) );
      break;
    }
  }





void SvCompiler::constValueLongCalc(SvValueBinaryLong *val)
  {
  constValueCalc( val->mOperand[0] );
  if( val->mOperand[0]->mConst ) {
    val->mConst = true;
    val->mConstInt = val->mOperand[0]->mConstInt;
    for( int i = 1; i < val->mOperCount; i++ ) {
      constValueCalc( val->mOperand[i] );
      if( !val->mOperand[i]->mConst ) {
        val->mConst = false;
        val->mConstInt = 0;
        return;
        }
      val->mConstInt = val->constOperation( val->mConstInt, val->mOperand[i]->mConstInt );
      }
    }
  }
