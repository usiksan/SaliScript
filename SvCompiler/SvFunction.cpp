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

    Реализация класса SvFunction функции
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvFunction::SvFunction() :
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
  mResultType(nullptr),  //Тип результата
  mType(nullptr),        //Тип данной функции
  mBody(nullptr),        //Тело функции
  mLocalAmount(0)        //Место под локальные переменные
  {

  }


SvFunction::SvFunction(const QString &name, SvType *result, const SvMark &mark) :
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
  mType(nullptr),        //Тип данной функции
  mBody(nullptr),        //Тело функции
  mLocalAmount(0)        //Место под локальные переменные
  {

  }



//Получить глобальное имя функции
QString SvFunction::globalName() const
  {
  return mName;
  }




//Добавить параметр, если возможно
bool SvFunction::addParam(SvVariable *param)
  {
  //Проверить наличие параметра
  if( mParams.isPresent(param->mName) ) {
    delete param;
    return false;
    }
  //Адрес параметру назначен при вызове DoSubType
//  param->mAddress = mParamSize;
//  mParamSize += param->getSize();
  mParams.addVariable( param );
  return true;
  }



//Установить комментарии к функции
void SvFunction::setRemark(const QString &prevRemark, const QString &afterRemark)
  {
  if( mRemark.isEmpty() ) {
    if( afterRemark.isEmpty() )
      mRemark = prevRemark;
    else
      mRemark = afterRemark;
    }
  }




//Сформировать листинг определения функции
QString SvFunction::listingDeclaration()
  {
  QString str("function declare ");
  //Тип результата
  str.append( mResultType->mSignature ).append(" ").append(globalName()).append("(\n");
  str.append( mParams.listing(1) );
  str.append(") end function declare\n");
  return str;
  }




//Сформировать листинг определения функции
QString SvFunction::listingDefinition()
  {
  QString str( listingDeclaration() );
  str.append("{\n");
  //Содержание тела
  if( mBody ) str.append( mBody->listing(0) );
  str.append("}\n");
  return str;
  }




