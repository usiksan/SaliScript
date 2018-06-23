/*
Проект "Компилятор"
Содержание
  Реализация класса TCompiler часть A
  Функции:
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

//Создает листинг псевдокода
QString
SrCompiler::Listing() {
  QString str;

  //Пролог
  str.append( QString(
                "//*****************************************************************\n"
                "// Script Compiler %1\n"
                "// Author: Sibilev A.S.\n"
                "// Web: www.rc.saliLab.com\n"
                "//*****************************************************************\n").arg(TVERSION) );

  //Список зарегистрированных типов
  str.append( "\n"
              "//==================\n"
              "// types list\n" );
  str.append( mTypeList.listing() );

  //Глобальные переменные
  str.append( "\n"
              "//==================\n"
              "// global variables\n" );
  str.append( mVarGlobal.listing(0) );

  //Глобальные функции
  str.append( "\n"
              "//==================\n"
              "//  functions\n" );
  str.append( mFunGlobal.listingDeclaration() );

  //Раздел описания тел функций
  str.append( "\n"
              "//==================\n"
              "//  functions bodies\n" );
  str.append( mFunGlobal.listingDefinition() );

  //Раздел иерархии сцен

  //Теперь список ошибок
  if( mErrorList.count() ) {
    //Ошибки при компиляции
    str.append( "\n"
                "//==================\n"
                "// There are errors\n" );

    str.append( errorList() );
    }
  else
    //No errors
    str.append( "\n"
                "Compilation success\n");
  return str;
  }

