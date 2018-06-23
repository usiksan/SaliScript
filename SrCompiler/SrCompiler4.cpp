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

    Реализация класса TCompiler часть 4 (препроцессор)
*/
#include "SrCompiler.h"

using namespace SrCompiler6;



/*
Функция "Пропустить пробелы"
Описание
  Пока не конец источника пропускаем пробелы
*/
void
SrCompiler::Blank() {
  while( !mEof ) {
    if( IsEoln() )
      DoPreprocess();
    else if( mLine.at(mPtr).isSpace() )
      ++mPtr;
    else break;
    }
  }




/*
Функция "Пропустить пробелы"
Описание
  Пока не конец строки
*/
void
SrCompiler::White() {
  while( !IsEoln() && mLine.at(mPtr).isSpace() ) ++mPtr;
  }




/*
Функция "Препроцессор"
Описание
  Вызывается для получения новой строки и производства макрорасширений
*/
void
SrCompiler::DoPreprocess() {
  while(true) {
    //Прочитать очередную строку
    IfLine();
    if( mEof ) return;
    //Расширить
    DoExpand();
    //Если пустая строка, то нужна новая строка
    if( mMacroLine.isEmpty() || IsEoln() ) continue;
    //Пропустить начальные пробелы
    White();
    //Проверить директивы препроцессора
    if( Match( "#include" ) )
      DoInclude();
    else if( Match( "#define" ) )
      DoDefine();
    else if( Match( "#error") ) {
      //Пропустить пробелы
      White();
      ErrorKillLine( mLine.mid(mPtr) );
      }
    else return;
    }
  }





/*
Функция "Чтение очередной строки, условная компиляция"
Описание
*/
void
SrCompiler::IfLine() {
  while(true) {
    //Прочитать очередную строку
    InLine();
    //Проверить конец источника
    if( mEof ) return;
    //Убрать начальные пробелы
    White();
    //Если строка пуста, то читать снова
    if( IsEoln() ) continue;
    if( Match( "#ifdef" ) ) {
      ++mIfLevel;
      if( mSkipLevel ) continue;
      White();
      if( !ScanName() )
        ErrorKillLine( QObject::tr("Error. Need name for #ifdef.") );
      if( !mMacroTable.contains( mName ) )
        mSkipLevel = mIfLevel;
      }
    else if( Match( "#ifndef" ) ) {
      ++mIfLevel;
      if( mSkipLevel ) continue;
      White();
      if( !ScanName() )
        ErrorKillLine( QObject::tr("Error. Need name for #ifndef.") );
      if( mMacroTable.contains( mName ) )
        mSkipLevel = mIfLevel;
      }
    else if( Match("#iflog") ) {
      if( mSkipLevel ) continue;
      White();
      if( !ScanName() )
        ErrorKillLine( QObject::tr("Error. Need name for #ifdef.") );
      if( !mMacroTable.contains( mName ) )
        continue;
      return;
      }
    else if( Match("#if") ) {
      ++mIfLevel;
      if( mSkipLevel ) continue;
      White();
      if( !BConstCompare() )
        mSkipLevel = mIfLevel;
      }
    else if( Match( "#else" ) ) {
      if( mIfLevel ) {
        if( mSkipLevel == mIfLevel ) mSkipLevel = 0;
        else if( mSkipLevel == 0 )   mSkipLevel = mIfLevel;
        }
      else ErrorKillLine( QObject::tr("Error. No corresponding #if for #else.") );
      }
    else if( Match( "#endif" ) ) {
      if( mIfLevel ) {
        if( mSkipLevel == mIfLevel ) mSkipLevel = 0;
        --mIfLevel;
        }
      else ErrorKillLine( QObject::tr("Error. No corresponding #if for #endif.") );
      }
    else if( !mSkipLevel ) return;
    }
  }







/*
Функция "Сканирование числа"
Описание
  Проверяет и сканирует целое число из стандартного входного потока
*/
bool
SrCompiler::ScanInt() {
  if( !IsEoln() && mLine.at(mPtr).isDigit() ) {
    mName.clear();
    while( !IsEoln() && mLine.at(mPtr).isDigit() )
      mName.append( mLine.at(mPtr++) );
    return true;
    }
  return false;
  }




/*
Функция "Сканирование имени"
Описание
  Проверяет и сканирует имя из стандартного входного потока
*/
bool
SrCompiler::ScanName() {
  if( IsEoln() )
    return false;
  if( mLine.at(mPtr).isLetter() || mLine.at(mPtr) == QChar('_') )
    return ScanNameImp( mPtr );
  return false;
  }






/*
Функция "Сканирование имени"
Описание
  Проверяет и сканирует имя из стандартного входного потока
*/
bool
SrCompiler::ScanNameImp( int &ptr ) {
  //Очистить mName
  mName.clear();
  //Накопить имя
  while( ptr < mLine.size() && (mLine.at(ptr).isLetterOrNumber() || mLine.at(mPtr) == QChar('_')) )
    mName.append( mLine.at(ptr++) );
  return true;
  }






/*
Функция "Проверка на совпадение"
Описание
  Проверяет на совпадение со строкой, при совпадении равная часть удаляется
*/
bool
SrCompiler::Match( const char *ptr ) {
  if( !ptr || IsEoln() ) return false;
  int sour = mPtr;
  while( *ptr ) { //Для всех символов входной строки
    if( sour >= mLine.size() ) return false;
    if( QChar(*ptr++) != mLine.at(sour++) ) return false;   //При несовпадении возврат Ложь
    }
  //Совпадение, обновить указатель
  mPtr = sour;
  return true;
  }





/*
Функция "Проверка на совпадение"
Описание
  Проверяет на совпадение с символом, при совпадении равная часть удаляется
*/
bool
SrCompiler::Match( char ch ) {
  if( !IsEoln() && mLine.at(mPtr) == QChar(ch) ) {
    mPtr++;
    return true;
    }
  return false;
  }






void
SrCompiler::DoInclude() {
  White();
  if( IsEoln() || (mLine.at(mPtr) != QChar('"') && mLine.at(mPtr) != QChar('<')) ) {
    ErrorKillLine( QObject::tr("Error. Need \" or <.") );
    return;
    }
  //Определить завершающий символ
  QChar ch = mLine.at(mPtr) == QChar('"') ? QChar('"') : QChar('>');
  //Считать имя файла
  mPtr++;
  mName.clear();
  //Сканировать в имя, пока не завершающий символ
  while( !IsEoln() && mLine.at(mPtr) != ch )
    mName.append( mLine.at(mPtr++) );
  //Проверить наличие завершающего символа
  if( !IsEoln() && mLine.at(mPtr) == ch ) {
    DoFile( mName, ch == QChar('"') );
    }
  else
    //Ошибка
    ErrorKillLine( Need( ch == QChar('"') ? QChar('\"') : QChar('>')) );
  }






void
SrCompiler::DoDefine() {
  White(); //Пропустить пробелы
  if( !ScanName() ) ErrorKillLine( QObject::tr("Error. Need name.") );
  else {
    if( mMacroTable.contains( mName ) )
      ErrorKillLine( QObject::tr("Error. Dual name definition \"%1\".").arg(mName) );
    else {
      //Разместить макро
      SrMacro *mac = new SrMacro();
      mac->mHelp.mDefinePos = mark();
      //Сканировать макрорасширение
      White();
      while( !IsEoln() )
        mac->mExpander.append( mLine.at(mPtr++) );
      //Разместить в таблице макро
      mMacroTable.insert( mName, mac );
      }
    }
  }









void
SrCompiler::DoExpand() {
  //Освободить расширенную строку
  mMacroLine.clear();
  mLineInt.clear();
  while( !mEof && !IsEoln() ) {

    //Коментарии стиля "//"
    if( mLine.at(mPtr) == QChar('/') && (mPtr+1 < mLine.size()) && mLine.at(mPtr+1) == QChar('/') ) {
      //Сохранить комментарии для редактора
      if( !mAfterRemark.isEmpty() ) {
        //Добавить в предыдущий коментарий
        mPrevRemark += mAfterRemark + QString("\n");
        }
      else mPrevRemark.clear();
      mAfterRemark = mLine.mid( mPtr+2 );
      break;
      }

    //Коментарии стиля /* */
    if( mLine.at(mPtr) == QChar('/') && (mPtr+1 < mLine.size()) && mLine.at(mPtr+1) == QChar('*') ) {
      //Пропустить коментарии
      mPtr += 2;
      while( !mEof && (IsEoln() || !(mLine.at(mPtr) == QChar('*') && (mPtr+1 < mLine.size()) && mLine.at(mPtr+1) == QChar('/'))) )
        if( !IsEoln() ) mPtr++;
        else InLine();
      if( !mEof ) mPtr += 2;
      mMacroLine.append( QChar(' ') );
      mLineInt.append( mPtr );
      }
    else if( mLine.at(mPtr) == QChar('"') ) {
      //Строка
      mMacroLine.append( mLine.at(mPtr++) );
      mLineInt.append( mPtr );
      while( !IsEoln() && mLine.at(mPtr) != QChar('"') ) {
        if( mLine.at(mPtr) == QChar('\\') ) {
          mMacroLine.append( mLine.at(mPtr++) );
          mLineInt.append(mPtr);
          }
        mMacroLine.append( mLine.at(mPtr++) );
        mLineInt.append(mPtr);
        }
      mMacroLine.append( QChar('"') );
      mLineInt.append( mPtr );
      if( IsEoln() || mLine.at(mPtr) != QChar('"') ) ErrorKillLine( QObject::tr("Error. String not closed.") );
      else mPtr++;
      }
    else if( ScanName() ) {
      //Проверить имя
      if( mMacroTable.contains( mName ) ) {
        //Макро имя найдено, подставить макрорасширение
        mMacroLine.append( mMacroTable.value(mName)->mExpander );
        //Добавить место, в котором потребовалось данное макрорасширение
        mMacroTable.value(mName)->mReferneceList.append( mark() );
        }
      else {
        //Макро не найдено, вернуть имя
        mMacroLine.append( mName );
        }
      while( mLineInt.count() < mMacroLine.count() )
        mLineInt.append( mPtr );
      }
    else {
      mMacroLine.append( mLine.at(mPtr++) );
      mLineInt.append( mPtr );
      }

    //Если дошли до конца строки без комментариев, то начинаем их (комментарии) удалять
    if( IsEoln() || mEof ) {
      //Если последующего комментария нету, значит это вторая строка без комментариев
      //поэтому удаляем предыдущий
      if( mAfterRemark.isEmpty() )
        mPrevRemark.clear();
      //Удаляем последующий комментарий
      mAfterRemark.clear();
      }
    }
  //Расширение строки завершено, переназначить вход
  mLine = mMacroLine;
  mPtr = 0;
  //Если нужно выдавать исходную строку в виде коментария, то выдать
  if( mCRemark ) AddAsmRemark( mMacroLine );
  }
