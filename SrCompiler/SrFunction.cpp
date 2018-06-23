/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvFunction - function
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrFunction::SrFunction() :
  mHelp(),               //Помощь по функции
  mName(),               //Имя функции (возможны безымянные функции)
  mDefined(false),       //Флаг, выставляется, когда тело функции уже определено
  mDeclared(false),      //Флаг, выставляется, когда функция объявлена (например в классе)
                         //в глобальном контексте все функции объявлены по умолчанию
  mAddress(0),           //Адрес начала кода
  mImportIndex(0),       //Индекс импортной функции
  mParamSize(0),         //Размер области параметров
  mMarkDefine(),         //Место определения
  mMarkDeclare(),        //Место первого объявления
  mRemark(),             //Описание переменной
  mParams(tsrLocal),     //Список параметров функции
  mResultType(0),        //Тип результата
  mType(0),              //Тип данной функции
  mBody(0),              //Тело функции
  mLocalAmount(0)        //Место под локальные переменные
  {

  }


SrFunction::SrFunction(const QString &name, SrType *result, const SrMark &mark) :
  mHelp(),               //Помощь по функции
  mName(name),           //Имя функции (возможны безымянные функции)
  mDefined(false),       //Флаг, выставляется, когда тело функции уже определено
  mDeclared(false),      //Флаг, выставляется, когда функция объявлена (например в классе)
                         //в глобальном контексте все функции объявлены по умолчанию
  mAddress(0),           //Адрес начала кода
  mImportIndex(0),       //Индекс импортной функции
  mParamSize(0),         //Размер области параметров
  mMarkDefine(mark),     //Место определения
  mMarkDeclare(mark),    //Место первого объявления
  mRemark(),             //Описание переменной
  mParams(tsrLocal),     //Список параметров функции
  mResultType(result),   //Тип результата
  mType(0),              //Тип данной функции
  mBody(0),              //Тело функции
  mLocalAmount(0)        //Место под локальные переменные
  {

  }



//Получить глобальное имя функции
QString SrFunction::globalName() const
  {
  return mName;
  }




//Добавить параметр, если возможно
bool SrFunction::addParam(SrVariable *param)
  {
  //Проверить наличие параметра
  if( mParams.isPresent(param->mName) ) {
    delete param;
    return false;
    }
  param->mAddress = mParamSize;
  mParamSize += param->getSize();
  mParams.addVariable( param );
  return true;
  }



//Установить комментарии к функции
void SrFunction::setRemark(const QString &prevRemark, const QString &afterRemark)
  {
  if( mRemark.isEmpty() ) {
    if( afterRemark.isEmpty() )
      mRemark = prevRemark;
    else
      mRemark = afterRemark;
    }
  }




//Сформировать листинг определения функции
QString SrFunction::listingDeclaration()
  {
  QString str("function declare ");
  //Тип результата
  str.append( mResultType->mSignature ).append(" ").append(globalName()).append("(\n");
  str.append( mParams.listing(1) );
  str.append(") end function declare\n");
  return str;
  }




//Сформировать листинг определения функции
QString SrFunction::listingDefinition()
  {
  QString str( listingDeclaration() );
  str.append("{\n");
  //Содержание тела
  if( mBody ) str.append( mBody->listing(0) );
  str.append("}\n");
  return str;
  }




