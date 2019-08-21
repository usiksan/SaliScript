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

    Реализация класса SvFunctionList - список функций
*/


#include "SvCompiler.h"

using namespace SvCompiler6;


SvFunctionList::SvFunctionList() :
  mList(),
  mHash()
  {

  }




SvFunctionList::~SvFunctionList()
  {
  qDeleteAll(mList);
  }





//Очистить список
void SvFunctionList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  }





//Добавить функцию к списку
void SvFunctionList::addFunction(SvFunctionPtr fun, const QString name )
  {
  mList.append( fun );
  mHash.insert( name, fun );
  }




//Заменить функцию в списке
SvFunction *SvFunctionList::addFunctionDefinition(SvFunctionPtr fun)
  {
  SvFunction *prev = mHash.value( fun->mName, 0 );
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
QString SvFunctionList::listingDeclaration()
  {
  QString str;
  for( SvFunction *fun : mList )
    str.append( fun->listingDeclaration() );
  return str;
  }




//Сформировать листинг функций с определениями
QString SvFunctionList::listingDefinition()
  {
  QString str;
  for( SvFunction *fun : mList )
    str.append( fun->listingDefinition() ).append("\n\n\n");
  return str;
  }

