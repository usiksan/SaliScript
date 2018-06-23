/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvVariableList - variable list
*/

#include "SrCompiler.h"

using namespace SrCompiler6;


SrVariableList::SrVariableList(SrSort sort) :
  mList(),
  mHash(),
  mSort(sort)
  {

  }





SrVariableList::~SrVariableList()
  {
  qDeleteAll(mList);
  }




//Очистить список
void SrVariableList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  }




void SrVariableList::addVariable(SrVariable *var)
  {
  //Добавить в список переменных
  mList.append( var );
  //Установить сорт переменной в соответствии с таблицей
  var->mSort = mSort;
  //Если еще нет в списке имен, то добавить
  if( !mHash.contains(var->mName) )
    mHash.insert( var->mName, var );
  }







void SrVariableList::addList(const SrVariableList &src)
  {
  for( SrVariablePtr var : src.mList ) {
    SrVariablePtr copy = new SrVariable( var );
    addVariable( copy );
    }
  }




//Получить листинг переменных
QString SrVariableList::listing(int level)
  {
  QString str;
  QString padding(level,QChar(' '));
  for( SrVariable *var : mList )
    str.append(padding).append( var->printListing() ).append("\n");
  return str;
  }


