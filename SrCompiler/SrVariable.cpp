/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvVariable - single variable
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

SrVariable::SrVariable() :
  mHelp(),               //Помощь по переменной
  mName(),               //Имя переменной
  mType(0),              //Тип переменной
  mSort(tsrGlobal),
  mAddress(0),           //Адрес начала относительный
  mArraySize(0),         //Размер массива (для массивов)
  mMarkDefine(),         //Место определения
  mRemark(),             //Описание переменной
  mInit(0)
  {

  }

SrVariable::SrVariable(const SrMark &src, const QString name, SrType *type, int addr, int arraySize) :
  mHelp(),               //Помощь по переменной
  mName(name),           //Имя переменной
  mType(type),           //Тип переменной
  mSort(tsrGlobal),
  mAddress(addr),        //Адрес начала относительный
  mArraySize(arraySize), //Размер массива (для массивов)
  mMarkDefine(src),      //Место определения
  mRemark(),             //Описание переменной
  mInit(0)
  {

  }

SrVariable::SrVariable(const SrVariable *var) :
  mHelp(var->mHelp),
  mName(var->mName),
  mType(var->mType),                //Тип переменной, тип результата для функции
  mSort(var->mSort),
  mAddress(var->mAddress),          //Адрес начала
  mArraySize(var->mArraySize),
  mMarkDefine(var->mMarkDefine),    //Место определения
  mRemark(var->mRemark),            //Описание переменной
  mInit(0)
  {
  }



SrVariable::~SrVariable()
  {
  if( mInit ) delete mInit;
  }







//Добавить помощь
void SrVariable::setRemark(const QString &prevRemark, const QString &afterRemark)
  {
  if( mRemark.isEmpty() ) {
    if( afterRemark.isEmpty() )
      mRemark = prevRemark;
    else
      mRemark = afterRemark;
    }
  }





//Получить размер переменной
int SrVariable::getSize()
  {
  return mType->mSize;
  }




QString SrVariable::printListing()
  {
  QString str = mType->mSignature;
  str.append( QString(" %1:%2").arg(mAddress).arg(mName) );
  return str;
  }





