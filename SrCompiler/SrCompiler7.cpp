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

    Реализация класса TCompiler часть 7 (таблицы)
*/
#include "SrCompiler.h"

using namespace SrCompiler6;


void
SrCompiler::AddAsmRemark( const QString &sour ) {
  //Создать триак с коментарием
//  AddOperation( toRemark, mTypeVoid, mRemarkTable.count(), 0 );
  //Скопировать коментарий в таблицу коментариев
  mRemarkTable.append( sour );
  }






bool SrCompiler::isGlobalSymbol(const QString &name)
  {
  //Проверить среди типов
  if( mTypeList.isPresent(name) )
    return true;
  //Проверить среди переменных
  if( mVarGlobal.isPresent(name) )
    return true;
  //Проверить среди функций
  return mFunGlobal.isPresent(name);
  }
