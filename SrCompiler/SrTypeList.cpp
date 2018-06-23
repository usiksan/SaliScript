/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvTypeList - type list
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

SrTypeList::SrTypeList() :
  mHash(),
  mSign(),
  mList()
  {

  }

SrTypeList::~SrTypeList()
  {
  qDeleteAll(mList);
  }




//Очистить список типов
void SrTypeList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  mSign.clear();
  }





//Получить указатель на тип
SrType *SrTypeList::getTypePointer(SrType *base)
  {
  SrType *ptr = new SrType();
  ptr->mName     = QString("*%1").arg(base->mName);
  ptr->mBaseType = base;
  ptr->mClass    = CLASS_POINTER;
  ptr->mNumElem  = 0;
  ptr->mSize     = 1;
  return addType( ptr );
  }







//Получить тип массив
SrType *SrTypeList::getTypeArray( SrType *base, int numElem )
  {
  SrType *arr = new SrType();
  arr->mName     = QString("%1[%2]").arg(base->mName).arg(numElem);
  arr->mBaseType = base;
  arr->mClass    = CLASS_ARRAY;
  arr->mNumElem  = numElem;
  arr->mSize     = base->mSize * numElem;
  return addType( arr );
  }






//Добавить тип к списку
SrType *SrTypeList::addType(SrType *type)
  {
  //Сформировать сигнатуру у добавляемого типа
  QString sign = type->buildSignature();

  //Проверить наличие типа с такой-же сигнатурой
  if( mSign.contains(sign) ) {
    //Найдено, добавляемый удалить и вернуть из списка
    delete type;
    return mSign.value(sign,0);
    }

  //Не найдено, добавить
  type->mTypeList = this; //Записать в тип указатель на список
  mList.append( type );
  mHash.insert( type->mName, type );
  mSign.insert( sign, type );
  return type;
  }





//Добавить имя для типа
void SrTypeList::addTypeName(SrType *type, const QString &name)
  {
  if( !mHash.contains(name) ) {
    mHash.insert( name, type );
    }
  }




//Сформировать листинг типов
QString SrTypeList::listing()
  {
  QString str;
  for( SrType *type : mList )
    str.append( type->listing() );
  return str;
  }




