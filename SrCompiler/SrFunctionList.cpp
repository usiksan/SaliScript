/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvFunctionList - list of functions
*/

#include "SrCompiler.h"

using namespace SrCompiler6;


SrFunctionList::SrFunctionList() :
  mList(),
  mHash()
  {

  }




SrFunctionList::~SrFunctionList()
  {
  qDeleteAll(mList);
  }





//Очистить список
void SrFunctionList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  }





//Добавить функцию к списку
void SrFunctionList::addFunction(SrFunctionPtr fun, const QString name )
  {
  mList.append( fun );
  mHash.insert( name, fun );
  }




//Заменить функцию в списке
SrFunction *SrFunctionList::addFunctionDefinition(SrFunctionPtr fun)
  {
  SrFunction *prev = mHash.value( fun->mName, 0 );
  if( prev ) {
    //Есть предварительное описание, которое нужно заменить на определение
    fun->mHelp.update( prev->mHelp );         //Помощь по функции
    fun->mDefined = prev->mDefined;           //Флаг, выставляется, когда тело функции уже определено
    fun->mDeclared = prev->mDeclared;         //Флаг, выставляется, когда функция объявлена (например в классе)
                                              //в глобальном контексте все функции объявлены по умолчанию
    fun->mMarkDeclare = prev->mMarkDeclare;   //Место первого объявления
    if( fun->mRemark.isEmpty() )
      fun->mRemark = prev->mRemark;           //Описание переменной

    //Заменить в списке функций
    int i = mList.indexOf( prev );
    mList[i] = fun;

    //Заменить в хэш
    mHash.insert( fun->mName, fun );

    //Удалить прежнюю функцию
    delete prev;
    }
  else {
    //Предыдущей функции нету, просто добавить
    addFunction( fun, fun->globalName() );
    }
  return fun;
  }






//Сформировать листинг описаний функций
QString SrFunctionList::listingDeclaration()
  {
  QString str;
  for( SrFunction *fun : mList )
    str.append( fun->listingDeclaration() );
  return str;
  }




//Сформировать листинг функций с определениями
QString SrFunctionList::listingDefinition()
  {
  QString str;
  for( SrFunction *fun : mList )
    str.append( fun->listingDefinition() ).append("\n\n\n");
  return str;
  }

