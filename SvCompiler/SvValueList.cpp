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

    Реализация класса SvValueList - список значений (для таблиц инициализации)
*/


#include "SvCompiler.h"

using namespace SvCompiler6;

QString SvValueList::listing()
  {
  QString str;
  for( int index = 0; index < mList.count(); index++ )
    if( mList.at(index) )
      str.append( QString("%1 : %2\n").arg(index).arg(mList.at(index)->listing()) );
    else
      str.append( QString("%1 : NULL\n").arg(index) );
  return str;
  }
