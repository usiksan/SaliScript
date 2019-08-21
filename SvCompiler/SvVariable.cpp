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

    Реализация класса SvVariable - переменная
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvVariable::SvVariable() :
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

SvVariable::SvVariable(const SvMark &src, const QString name, SvType *type, int addr, int arraySize) :
  mHelp(),               //Помощь по переменной
  mName(name),           //Имя переменной
  mType(type),           //Тип переменной
  mSort(tsrGlobal),
  mAddress(addr),        //Адрес начала относительный
  mArraySize(arraySize), //Размер массива (для массивов)
  mMarkDefine(src),      //Место определения
  mRemark(),             //Описание переменной
  mInit(nullptr)
  {

  }

SvVariable::SvVariable(const SvVariable *var) :
  mHelp(var->mHelp),
  mName(var->mName),
  mType(var->mType),                //Тип переменной, тип результата для функции
  mSort(var->mSort),
  mAddress(var->mAddress),          //Адрес начала
  mArraySize(var->mArraySize),
  mMarkDefine(var->mMarkDefine),    //Место определения
  mRemark(var->mRemark),            //Описание переменной
  mInit(nullptr)
  {
  }



SvVariable::~SvVariable()
  {
  if( mInit ) delete mInit;
  }







//Добавить помощь
void SvVariable::setRemark(const QString &prevRemark, const QString &afterRemark)
  {
  if( mRemark.isEmpty() ) {
    if( afterRemark.isEmpty() )
      mRemark = prevRemark;
    else
      mRemark = afterRemark;
    }
  }





//Получить размер переменной
int SvVariable::getSize()
  {
  return mType->mSize;
  }




QString SvVariable::printListing()
  {
  QString str = mType->mSignature;
  str.append( QString(" %1:%2").arg(mAddress).arg(mName) );
  return str;
  }





