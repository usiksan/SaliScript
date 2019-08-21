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

    Реализация класса SvTypeList - список типов
*/


#include "SvCompiler.h"

using namespace SvCompiler6;

SvTypeList::SvTypeList() :
  mHash(),
  mSign(),
  mList()
  {

  }

SvTypeList::~SvTypeList()
  {
  qDeleteAll(mList);
  }




//Очистить список типов
void SvTypeList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  mSign.clear();
  }





//Получить указатель на тип
SvType *SvTypeList::getTypePointer(SvType *base)
  {
  SvType *ptr = new SvType();
  ptr->mName     = QString("*%1").arg(base->mName);
  ptr->mBaseType = base;
  ptr->mClass    = CLASS_POINTER;
  ptr->mNumElem  = 0;
  ptr->mSize     = 1;
  return addType( ptr );
  }







//Получить тип массив
SvType *SvTypeList::getTypeArray( SvType *base, int numElem )
  {
  SvType *arr = new SvType();
  arr->mName     = QString("%1[%2]").arg(base->mName).arg(numElem);
  arr->mBaseType = base;
  arr->mClass    = CLASS_ARRAY;
  arr->mNumElem  = numElem;
  arr->mSize     = base->mSize * numElem;
  return addType( arr );
  }






//Добавить тип к списку
SvType *SvTypeList::addType(SvType *type)
  {
  //Сформировать сигнатуру у добавляемого типа
  QString sign = type->buildSignature();

  //Проверить наличие типа с такой-же сигнатурой
  if( mSign.contains(sign) ) {
    //Найдено, добавляемый удалить и вернуть из списка
    delete type;
    return mSign.value( sign, nullptr );
    }

  //Не найдено, добавить
  type->mTypeList = this; //Записать в тип указатель на список
  mList.append( type );
  mHash.insert( type->mName, type );
  mSign.insert( sign, type );
  return type;
  }





//Добавить имя для типа
void SvTypeList::addTypeName(SvType *type, const QString &name)
  {
  if( !mHash.contains(name) ) {
    mHash.insert( name, type );
    }
  }




//Сформировать листинг типов
QString SvTypeList::listing()
  {
  QString str;
  for( SvType *type : mList )
    str.append( type->listing() );
  return str;
  }




