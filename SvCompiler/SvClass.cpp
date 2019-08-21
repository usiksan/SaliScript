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
    Часть скриптового языка rc++
    struct
*/
#include "SvCompiler.h"

using namespace SvCompiler6;

//Конструктор обычного класса
SvStruct::SvStruct(const QString name) :
  SvType( name, CLASS_STRUCT, 0, nullptr ),
  mDefined(false),    //Структура определена
  mBaseStruct(nullptr),      //Базовый тип
  mMemberList(tsrMember)
  {

  }






SvStruct::~SvStruct()
  {
  }





void SvStruct::setBase(SvStruct *base)
  {
  mBaseStruct = base;
  //Скопировать список членов
  mMemberList.addList( base->mMemberList );
  //Скопировать размер
  mSize = base->mSize;
  }





void SvStruct::addMember(SvVariable *mem)
  {
  mem->mAddress = mSize;
  mSize += mem->mType->mSize;
  mMemberList.addVariable(mem);
  }






//Построить листинг
QString SvStruct::listing()
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


