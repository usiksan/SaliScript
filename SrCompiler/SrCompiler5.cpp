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

    Реализация класса TCompiler часть 5 (препроцессор - константные выражения)
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

int
SrCompiler::BConstCompare() {
  return 0;
/*  if( Match("!") ) return !BConstCompare();
  if( Match("(") ) {
    int r = BConstCompare();
    if( !Match(")") ) {
      ErrorKillLine( QObject::tr("Error. Need ) in #if expression.") );
      return 0;
      }
    return r;
    }
  if( ScanName() ) {

    int left = BConstCompare();
    int op;
    while( !Match(")") ) {
      if( Match("==") ) op = 0;
      else if( Match("!=") ) op = 1;
      else if( Match("<=") ) op = 2;
      else if( Match(">=") ) op = 3;
      else if( Match("<") ) op = 4;
      else if( Match(">") ) op = 5;
      else if( Match("||") ) op = 6;
      else if( Match("&&") ) op = 7;
      else if( Match("|") ) op = 8;
      else if( Match("&") ) op = 9;
      else {
        ErrorKillLine( QObject::tr("Error. Invalid expression for #if.") );
        return 0;
        }
      int right = BConstCompare();
      switch( op ) {

        }
      }
    }
    */
  }

/*
B1 операция ||
*/
int
SrCompiler::BccB1() {
  int val = BccB2();
  while( Match("||") ) {
    val = val || BccB2();
    }
  return val;
  }



/*
B2 операция &&
*/
int
SrCompiler::BccB2() {
  int val = BccB3();
  while( Match("&&") ) {
    val = val && BccB3();
    }
  return val;
  }



/*^
B3 операция |
*/
int
SrCompiler::BccB3() {
  int val = BccB4();
  while( Match("|") ) {
    val = val | BccB4();
    }
  return val;
  }



/*^
B4 операция ^
*/
int
SrCompiler::BccB4() {
  int val = BccB5();
  while( Match("^") ) {
    val = val ^ BccB5();
    }
  return val;
  }




/*^
B5 операция &
*/
int
SrCompiler::BccB5() {
  int val = BccB6();
  while( Match("&") ) {
    val = val & BccB6();
    }
  return val;
  }




/*
B6 операция == и !=
*/
int
SrCompiler::BccB6() {
  int val = BccB7();
  while( 1 ) {
    if( Match("==") ) val = val == BccB7();
    else if( Match("!=") ) val = val != BccB7();
    else break;
    }
  return val;
  }




/*
B7 операция <=  >=  <  >
*/
int
SrCompiler::BccB7() {
  int val = BccB8();
  while( 1 ) {
    if( Match("<=") ) val = val <= BccB8();
    else if( Match(">=") ) val = val >= BccB8();
    else if( Match("<") ) val = val < BccB8();
    else if( Match(">") ) val = val > BccB8();
    else break;
    }
  return val;
  }




/*^
B8 операция << и >>
*/
int
SrCompiler::BccB8() {
  int val = BccB9();
  while( 1 ) {
    if( Match("<<") ) val = val << BccB9();
    else if( Match(">>") ) val = val >> BccB9();
    else break;
    }
  return val;
  }




/*^
B9 операция + и -
*/
int
SrCompiler::BccB9() {
  int val = BccB10();
  while( 1 ) {
    if( Match("+") ) val = val + BccB10();
    else if( Match("-") ) val = val - BccB10();
    else break;
    }
  return val;
  }





/*
B10 операция *  /  %
*/
int
SrCompiler::BccB10() {
  int val = BccB11();
  while( 1 ) {
    if( Match("*") ) val = val * BccB11();
    else if( Match("/") ) val = val / BccB11();
    else if( Match("%") ) val = val % BccB11();
    else break;
    }
  return val;
  }





/*
B11 операция () значение
*/
int
SrCompiler::BccB11() {
  White(); //Пропустить пробелы
  if( Match("(") ) {
    int val = BccB1();
    if( !Match(")") ) {
      ErrorKillLine(  QObject::tr("Error. Need closing bracket ')'.") );
      return 1;
      }
    return val;
    }

  if( Match("defined") ) {
    White(); //Пропустить пробелы
    if( !Match("(") ) {
      ErrorKillLine(  QObject::tr("Error. Need open bracket '('.") );
      return 1;
      }
    White(); //Пропустить пробелы
    if( !ScanName() ) {
      ErrorKillLine(  QObject::tr("Error. Need name.") );
      return 1;
      }
    White(); //Пропустить пробелы
    if( !Match(")") ) {
      ErrorKillLine(  QObject::tr("Error. Need closing bracket ')'.") );
      return 1;
      }
    return mMacroTable.contains( mName ) ? 1 : 0;
    }

  //Проверить идентификатор
//  if( ScanName() ) {
//    if( mMacroTable.contains( mName ) ) {

//      }
//    }
  return 0;
  }
