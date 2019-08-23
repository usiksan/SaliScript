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

    Реализация класса SvType - базовый тип
*/

#include "SvCompiler.h"

using namespace SvCompiler6;

SvType::SvType() :
  mName("empty type"),           //Первое имя типа
  mSignature(),                  //Сигнатура типа
  mClass(0),                     //Основной тип (класс)
  mNumElem(0),                   //Количество элементов массива
  mSize(0),                      //Размер объекта данного типа
  mBaseType(0),                  //Базовый тип для производных типов
  mHelp(),                       //Помощь по типу
  mTypeList(0)
  {
  buildSignature();
  }




SvType::~SvType()
  {

  }





SvType::SvType(const QString &name, int svClass, int size, SvType *base) :
  mName(name),
  mClass(svClass),
  mNumElem(0),
  mSize(size),
  mBaseType(base)
  {
  }








SvStruct *SvType::toStruct()
  {
  return dynamic_cast<SvStruct*>(this);
  }



SvFunctionType *SvType::toFunction()
  {
  return dynamic_cast<SvFunctionType*>(this);
  }









//Получить указатель на тип
SvType *SvType::getTypePointer()
  {
  if( mTypeList )
    return mTypeList->getTypePointer( this );
  return nullptr;
  }







//Получить тип массив
SvType *SvType::getTypeArray(int numElem)
  {
  if( mTypeList )
    return mTypeList->getTypeArray( this, numElem );
  return nullptr;
  }




bool SvType::isMatchParam(SvType *src, bool srcNull )
  {
  if( src == this ) return true;
  if( isPointer() && src->isInt() && srcNull ) return true;
  if( isPointer() && (src->isArray() || src->isPointer() ) ) {
    //Указателю присваивается массив или указатель

    //Указателю на void можно присвоить любой указатель
    if( mBaseType->isVoid() ) return true;

    //Указателю на структуру можно присвоить указатель на дочернюю структуру
    if( mBaseType->isStruct() && src->mBaseType->isStruct() ) {
      //Ищем среди родительских структур источника данную структуру
      for( SvType *base = src->mBaseType; base; base = base->mBaseType )
        if( base == mBaseType ) return true;
      }

    //Указателю на функцию можно присвоить указатель на функцию с такой-же сигнатурой
    if( mBaseType->isFunction() && src->mBaseType->isFunction() ) {
      //Сравниваем сигнатуры функций
      return mBaseType->mSignature == src->mBaseType->mSignature;
      }
    }

  //Указателю на функцию можно присвоить функцию
  if( isPointer() && mBaseType->isFunction() && src->isFunction() ) {
    //Сравниваем сигнатуры функций
    return mBaseType->mSignature == src->mSignature;
    }

  return false;
  }



//Построить сигнатуру
QString SvType::buildSignature()
  {
  switch( mClass ) {
    case CLASS_VOID   :
      mSignature = QString("void");
      break;
    case CLASS_INT    :   //Целое 32 бит
      mSignature = QString("int");
      break;
    case CLASS_STRUCT   :   //Структура
      mSignature = QString(":") + mName;
      break;
    case CLASS_CSTRING   :   //Константная строка
      mSignature = QString("cstring");
      break;
    case CLASS_CBLOCK   :   //Блок константных данных (картинка, звук и т.п.)
      mSignature = QString("cblock");
      break;
    case CLASS_ARRAY    :
      mSignature = QString("[]");
      break;
    case CLASS_POINTER  :   //Указатель на что-нибудь
      mSignature = QString("*");
      break;
    default :
      mSignature = QString("undef");
      break;
    }
  if( mBaseType )
    mSignature.append( mBaseType->mSignature );
  return mSignature;
  }




//Построить листинг
QString SvType::listing()
  {
  QString str(mName);
  str.append(" ").append(mSignature).append("\n");
  return str;
  }











