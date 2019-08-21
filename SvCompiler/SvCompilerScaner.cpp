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

    Реализация класса TCompiler часть 6 (сканер)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;



quint32
ConvertDigit( char ch ) {
  if( ch >= '0' && ch <= '9' ) return ch - '0';
  if( ch >= 'a' && ch <= 'f' ) return ch - 'a' + 10;
  if( ch >= 'A' && ch <= 'F' ) return ch - 'A' + 10;
  return 0;
  }




bool
IsHex( QChar qch ) {
  char ch = qch.toLower().toLatin1();
  if( ch >= '0' && ch <= '9' ) return true;
  if( ch >= 'a' && ch <= 'f' ) return true;
  return false;
  }




bool
SvCompiler::ScanDigit() {
  if( mLine.at( mPtr ).isDigit() ) {
    quint32 value = 0;
    //Проверить ведущий нуль
    if( Match( '0' ) ) {
      //Проверить 16
      if( Match( 'x' ) || Match( 'X' ) ) {
        while( !IsEoln() && IsHex(mLine.at( mPtr )) ) {
          value <<= 4;
          value += ConvertDigit( mLine.at( mPtr++ ).toLatin1() );
          }
        }
      else {
        //8
        while( !IsEoln() && mLine.at( mPtr ).isDigit() ) {
          value <<= 3;
          value += static_cast<quint32>( mLine.at( mPtr++ ).digitValue() );
          }
        }
      }
    else {
      //10
      while( !IsEoln() && mLine.at( mPtr ).isDigit() ) {
        value *= 10;
        value += static_cast<quint32>( mLine.at( mPtr++ ).digitValue() );
        }
      }
    if( Match( '.' ) ) {
      mToken.mId     = ttFloat; //С плавающей точкой
      mToken.mDValue = value;   //Целая часть
      //Число с плавающей точкой
      double val = 0;
      double pos = 0.1;
      while( !IsEoln() && mLine.at( mPtr ).isDigit() ) {
        val += pos * mLine.at( mPtr++ ).digitValue();
        pos /= 10.0;
        }
      mToken.mDValue += val; //Дробная часть
      if( Match( 'e' ) || Match( 'E' ) ) {
        //Есть экспонента
        int exp = 0;
        int sign = 1;
        if( Match( '+' ) ) sign = 1;
        else if( Match( '-' ) ) sign = 0;
        while( !IsEoln() && mLine.at( mPtr ).isDigit() ) {
          exp *= 10;
          exp += mLine.at( mPtr++ ).digitValue();
          }
        //Теперь возвести в степень
        while( exp-- )
          if( sign ) mToken.mDValue *= 10.0;
          else       mToken.mDValue /= 10.0;
        }
      }
    else {
      //Целое число
      mToken.mUValue = value;
      mToken.mId     = ttInt;
      mToken.mUnsigned = 0;
      mToken.mLong     = 0;
      if( Match( 'u' ) || Match( 'U' ) ) mToken.mUnsigned = 1;
      if( Match( 'l' ) || Match( 'L' ) ) mToken.mLong = 1;
      }
    return true;
    }
  return false;
  }






bool
SvCompiler::NextToken() {
  //Пропустить пробелы
  Blank();
  //Проверить конец файла
  if( mEof ) {
    mToken.mId = ttEof;
    return false;
    }
  mToken.mId   = ttUndefined;
  mToken.mType = 0;
  //Проверить идентификатор
  if( ScanName() ) {
    //Считался идентификатор, проверить в типах
    mToken.mString = mName;
    if( mTypeList.isPresent(mName) ) {
      //Это тип
      mToken.mType = mTypeList.getType(mName);
      mToken.mId   = ttType;
      }
    //проверить в ключевых словах
    else if( mKeyWords.contains(mName) ) {
      //Это ключевое слово
      mToken.mId = mKeyWords.value(mName);
      }
    else {
      //Это левый идентификатор
      mToken.mId     = ttName;
      }
    }
  else {
    if( !IsEoln() && mLine.at( mPtr ) == QChar( '"' ) ) {
      int value,i;
      //Это строка
      mToken.mId = ttString;  //Объявить токен
      mToken.mString.clear(); //Очистить строку

      //Все строки разделенные пробелами объединять
      while( !IsEoln() && mLine.at( mPtr ) == QChar( '"' ) ) {
        mPtr++; //Пропустили "
        while( !IsEoln() && mLine.at( mPtr ) != QChar( '"' ) ) {
          if( mLine.at( mPtr ) == QChar( '\\' ) ) {
            mPtr++;
//            CPChar TEscape = "\n\t\v\b\r\f\a";
            switch( mLine.at( mPtr ).toLatin1() ) {
              case '\\' :
              case '?'  :
              case '\'' :
              case '\"' : mToken.mString.append( mLine.at( mPtr++ ) ); break;
              case 'n'  : mToken.mString.append( QChar('\n') ); mPtr++; break;
              case 't'  : mToken.mString.append( QChar('\t') ); mPtr++; break;
              case 'v'  : mToken.mString.append( QChar('\v') ); mPtr++; break;
              case 'b'  : mToken.mString.append( QChar('\b') ); mPtr++; break;
              case 'r'  : mToken.mString.append( QChar('\r') ); mPtr++; break;
              case 'f'  : mToken.mString.append( QChar('\f') ); mPtr++; break;
              case 'a'  : mToken.mString.append( QChar('\a') ); mPtr++; break;
              case '0' : case '1' : case '2' : case '3' :
              case '4' : case '5' : case '6' : case '7' :
                //Символ - восьмиричное число
                value = i = 0;
                while( i < 3 && mLine.at( mPtr ).isDigit() ) {
                  value <<= 3;
                  value += mLine.at( mPtr++ ).digitValue();
                  i++;
                  }
                mToken.mString.append( QChar(value) );
                break;
              case 'x' :
              case 'X' :
                mPtr++;
                //Символ - шестнадцатиричное число
                value = 0;
                while( mLine.at( mPtr ).isNumber() ) {
                  value <<= 4;
                  value += mLine.at( mPtr++ ).digitValue();
                  }
                mToken.mString.append( QChar(value) );
                break;
              default : Error( QObject::tr("Error. Invalid symbol '%1'.").arg(mLine.at( mPtr )) ); return true;
              }
            }
          else mToken.mString.append( mLine.at( mPtr++ ) );
          }
        //Строка закончена
        if( !IsEoln() ) mPtr++; //Пропустить завершающую ковычку
        else { Error( QObject::tr("Error. Unclosed string.") ); break; }
        Blank();
        }
      }
    //Проверить символ
    else if( !IsEoln() && mLine.at( mPtr ) == QChar( '\'' ) )
      {
      //Одиночный символ
      mPtr++;
      if( !IsEoln() ) {
        mToken.mIValue = mLine.at( mPtr++ ).unicode();
        mToken.mId     = ttInt;
        if( IsEoln() || mLine.at( mPtr++ ) != QChar( '\'' ) )
          Error( QObject::tr("Error. Unclosed char.") );
        }
      else
        Error( QObject::tr("Error. Unclosed char.") );
      }
    //Проверить цифру
    else if( !ScanDigit() ) {
      //Это какой-то знак
      QChar ch = mLine.at(mPtr++);
      switch( ch.toLatin1() ) {
        case '{' : mToken.mId = tsFOpen; break;       // {
        case '}' : mToken.mId = tsFClose; break;      // }
        case '[' : mToken.mId = tsSOpen; break;       // [
        case ']' : mToken.mId = tsSClose; break;      // ]
        case '(' : mToken.mId = tsOpen; break;        // (
        case ')' : mToken.mId = tsClose; break;       // )
        case ';' : mToken.mId = tsSemicolon; break;   // ;
        case '?' : mToken.mId = tsQuestion; break;    // ?
        case ',' : mToken.mId = tsComma; break;       // ,
        case '~' : mToken.mId = tsNot; break;         // ~
        case ':' :
          mToken.mId = tsColon;       // :
          if( Match( ':' ) ) mToken.mId = tsDColon;
          break;
        case '.' :
          mToken.mId = tsPoint;       // .
          if( Match( '*' ) ) mToken.mId = tsPointMul;    // .*
          else if( mPtr + 1 < mLine.length() && mLine.at(mPtr) == QChar('.') && mLine.at( mPtr+1 ) == QChar('.') ) {
            mPtr += 2;
            mToken.mId = tsMultiPoint;  // ...
            }
          break;
        case '+' :
          mToken.mId = tsPlus;        // +
          if( Match( '=' ) ) mToken.mId = tsPlusAsg;     // +=
          else if( Match( '+' ) ) mToken.mId = tsInc;    // ++
          break;
        case '-' :
          mToken.mId = tsMinus;       // -
          if( Match( '=' ) ) mToken.mId = tsMinusAsg;    // -=
          else if( Match( '-' ) ) mToken.mId = tsDec;    // --
          else if( Match( '>' ) ) {
            mToken.mId = tsRef;         // ->
            if( Match( '*' ) ) mToken.mId = tsRefMul;    // ->*
            }
          break;
        case '*' :
          mToken.mId = tsMul;         // *
          if( Match( '=' ) ) mToken.mId = tsMulAsg;      // *=
          break;
        case '/' :
          mToken.mId = tsDiv;         // /
          if( Match( '=' ) ) mToken.mId = tsDivAsg;      // /=
          break;
        case '%' :
          mToken.mId = tsMod;         // %
          if( Match( '=' ) ) mToken.mId = tsModAsg;      // %=
          break;
        case '^' :
          mToken.mId = tsXor;         // ^
          if( Match( '=' ) ) mToken.mId = tsXorAsg;      // ^=
          break;
        case '&' :
          mToken.mId = tsAnd;         // &
          if( Match( '=' ) ) mToken.mId = tsAndAsg;      // &=
          else if( Match( '&' ) ) mToken.mId = tsLAnd;   // &&
          break;
        case '|' :
          mToken.mId = tsOr;          // |
          if( Match( '=' ) ) mToken.mId = tsOrAsg;       // |=
          else if( Match( '|' ) ) mToken.mId = tsLOr;    // ||
          break;
        case '!' :
          mToken.mId = tsLNot;        // !
          if( Match( '=' ) ) mToken.mId = tsNotEqu;      // !=
          break;
        case '=' :
          mToken.mId = tsAssign;      // =
          if( Match( '=' ) ) mToken.mId = tsEqu;         // ==
          break;
        case '<' :
          if( Match( '=' ) ) mToken.mId = tsLessEqu;     // <=
          else if( Match( '<' ) ) {
            if( Match( '=' ) ) mToken.mId = tsLShiftAsg;   // <<=
            else               mToken.mId = tsLShift;      // <<
            }
          else mToken.mId = tsLess;        // <
          break;
        case '>' :
          if( Match( '=' ) ) mToken.mId = tsGrowEqu;     // >=
          else if( Match( '>' ) ) {
            if( Match( '=' ) ) mToken.mId = tsRShiftAsg;   // >>=
            else               mToken.mId = tsRShift;      // >>
            }
          else mToken.mId = tsGrow;        // >
          break;
        default : Error( QObject::tr("Error. Invalid symbol '%1'.").arg(ch) );
        }
      }
    }
  mFileId        = mToken.mFileId;
  mLineIndex     = mToken.mLine;
  mToken.mFileId = mSourceStack.top()->FileId();
  mToken.mLine   = mSourceStack.top()->Line();
  return true;
  }




void
SvCompiler::NeedSemicolon() {
  if( mToken != tsSemicolon )
    ErrorEndSt( QObject::tr("Error. Need semicolon.") );
  else NextToken();
  }

