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

SrType::SrType() :
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




SrType::~SrType()
  {

  }





SrType::SrType(const QString &name, int svClass, int size, SrType *base) :
  mName(name),
  mClass(svClass),
  mNumElem(0),
  mSize(size),
  mBaseType(base)
  {
  }





bool SrType::canAssign(SrType *src)
  {
  //Если типы совпадают, то назначать можно
  if( src == this ) return true;

  //В остальных случаях проверим указатель
  if( src && mClass == TTYPE_POINTER && src->mClass == TTYPE_POINTER ) {
    if( mBaseType->mClass == TTYPE_STRUCT && src->mBaseType->mClass == TTYPE_STRUCT ) {
      //Подняться по базовым классам
      SrStruct *thisClass = mBaseType->toStruct();
      SrStruct *srcClass  = src->mBaseType->toStruct();
      Q_ASSERT( thisClass != 0 );
      while( srcClass != 0 ) {
        if( thisClass == srcClass ) return true;
        //Подняться к очередному базовому классу
        srcClass = srcClass->mBaseStruct;
        }
      }
    }
  return false;
  }



SrStruct *SrType::toStruct()
  {
  return dynamic_cast<SrStruct*>(this);
  }



SrFunctionType *SrType::toFunction()
  {
  return dynamic_cast<SrFunctionType*>(this);
  }









//Получить указатель на тип
SrType *SrType::getTypePointer()
  {
  if( mTypeList )
    return mTypeList->getTypePointer( this );
  return 0;
  }




//Получить указатель на свойство
SrType *SrType::getTypePropPointer()
  {
  if( mTypeList )
    return mTypeList->getTypePropPointer( this );
  return 0;
  }






//Получить тип массив
SrType *SrType::getTypeArray(int numElem)
  {
  if( mTypeList )
    return mTypeList->getTypeArray( this, numElem );
  return 0;
  }




bool SrType::isMatchParam(SrType *src, bool srcNull )
  {
  if( src == this ) return true;
  if( isPointer() && src->isInt() && srcNull ) return true;
  if( isPointer() && (src->isArray() || src->isPointer()) ) {
    //Указателю присваивается массив или указатель

    //Указателю на void можно присвоить любой указатель
    if( mBaseType->isVoid() ) return true;

    //Указателю на структуру можно присвоить указатель на дочернюю структуру
    if( mBaseType->isStruct() && src->mBaseType->isStruct() ) {
      //Ищем среди родительских структур источника данную структуру
      for( SrType *base = src->mBaseType; base; base = base->mBaseType )
        if( base == mBaseType ) return true;
      }

    //Указателю на объект можно присвоить указатель на дочерний объект
    else if( mBaseType->isObject() && src->mBaseType->isObject() ) {
      //Ищем среди родительских объектов источника данный объект
      for( SrType *base = src->mBaseType; base; base = base->mBaseType )
        if( base == mBaseType ) return true;
      }
    }
  return false;
  }



//Построить сигнатуру
QString SrType::buildSignature()
  {
  switch( mClass ) {
    case TTYPE_VOID   :
      mSignature = QString("void");
      break;
    case TTYPE_INT    :   //Целое 32 бит
      mSignature = QString("int");
      break;
    case TTYPE_STRUCT   :   //Структура
      mSignature = QString(":") + mName;
      break;
    case TTYPE_CSTRING   :   //Константная строка
      mSignature = QString("cstring");
      break;
    case TTYPE_CBLOCK   :   //Блок константных данных (картинка, звук и т.п.)
      mSignature = QString("cblock");
      break;
    case TTYPE_ARRAY    :
      mSignature = QString("[]");
      break;
    case TTYPE_POINTER  :   //Указатель на что-нибудь
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
QString SrType::listing()
  {
  QString str(mName);
  str.append(" ").append(mSignature).append("\n");
  return str;
  }











