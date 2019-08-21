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

    Реализация класса SvVariableList - список переменных
*/


#include "SvCompiler.h"

using namespace SvCompiler6;


SvVariableList::SvVariableList(SvSort sort) :
  mList(),
  mHash(),
  mSort(sort)
  {

  }





SvVariableList::~SvVariableList()
  {
  qDeleteAll(mList);
  }




//Очистить список
void SvVariableList::clear()
  {
  qDeleteAll(mList);
  mList.clear();
  mHash.clear();
  }




void SvVariableList::addVariable(SvVariable *var)
  {
  //Добавить в список переменных
  mList.append( var );
  //Установить сорт переменной в соответствии с таблицей
  var->mSort = mSort;
  //Если еще нет в списке имен, то добавить
  if( !mHash.contains(var->mName) )
    mHash.insert( var->mName, var );
  }







void SvVariableList::addList(const SvVariableList &src)
  {
  for( SvVariablePtr var : src.mList ) {
    SvVariablePtr copy = new SvVariable( var );
    addVariable( copy );
    }
  }




//Получить листинг переменных
QString SvVariableList::listing(int level)
  {
  QString str;
  QString padding(level,QChar(' '));
  for( SvVariable *var : mList )
    str.append(padding).append( var->printListing() ).append("\n");
  return str;
  }


