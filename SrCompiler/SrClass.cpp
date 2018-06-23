/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

//Конструктор обычного класса
SrStruct::SrStruct(const QString name) :
  SrType( name, CLASS_STRUCT, 0, 0 ),
  mDefined(false),    //Структура определена
  mBaseStruct(0),      //Базовый тип
  mMemberList(tsrMember)
  {

  }






SrStruct::~SrStruct()
  {
  }





void SrStruct::setBase(SrStruct *base)
  {
  mBaseStruct = base;
  //Скопировать список членов
  mMemberList.addList( base->mMemberList );
  //Скопировать размер
  mSize = base->mSize;
  }





void SrStruct::addMember(SrVariable *mem)
  {
  mem->mAddress = mSize;
  mSize += mem->mType->mSize;
  mMemberList.addVariable(mem);
  }






//Построить листинг
QString SrStruct::listing()
  {
  QString str("class ");
  str.append(mName);
  if( mBaseStruct ) str.append(" public ").append(mBaseStruct->mName);
  str.append(" {\n"
             "members:\n");
  //Список членов-переменных
  str.append( mMemberList.listing(1) );
  return str;
  }


