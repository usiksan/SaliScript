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
#include "SrCompiler.h"

using namespace SrCompiler6;

void SrCompiler::constValueCalc( SrValue *val )
  {
  if( val == nullptr ) return;
  //val->mType = mTypeInt;
  switch( val->code() ) {

    case svvVariable :       //Переменная (глобальная, локальная, параметр)
    case svvFunction :       //Функция (глобальная)
    case svvWaitFun :        //Специальная функция srWait
    case svvThrowFun :       //Специальная функция srThrow
    case svvCatchFun :       //Специальная функция srCatch
    case svvExceptionFun :   //Специальная функция srException
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
      constValueCalc( dynamic_cast<SrValueBitNot*>(val)->mOperand );
      dynamic_cast<SrValueBitNot*>(val)->checkConst();
      break;

    case svvLogNot :
      constValueCalc( dynamic_cast<SrValueLogNot*>(val)->mOperand );
      dynamic_cast<SrValueLogNot*>(val)->checkConst();
      break;

    case svvNeg :
      constValueCalc( dynamic_cast<SrValueNeg*>(val)->mOperand );
      dynamic_cast<SrValueNeg*>(val)->checkConst();
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
      constValueCalc( dynamic_cast<SrValueBinary*>(val)->mOperand1 );
      constValueCalc( dynamic_cast<SrValueBinary*>(val)->mOperand2 );
      dynamic_cast<SrValueBinary*>(val)->checkConst();
      break;

    case svvOr :
    case svvAnd :
    case svvXor :
      constValueLongCalc( dynamic_cast<SrValueBinaryLong*>(val) );
      break;
    }
  }





void SrCompiler::constValueLongCalc(SrValueBinaryLong *val)
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
