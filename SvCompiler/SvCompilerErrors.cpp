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

    Реализация класса TCompiler часть 3 (поток ошибок)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;

/*
Функция "Выдача ошибок"
*/
void
SvCompiler::Error( const QString &err ) {
  //Вывести сообщение о имени файла и номере ошибки
  ErrorInLine( err, mToken.mFileId, mToken.mLine );
  }


//Зафиксировать ошибку в заданном файле и строке
void SvCompiler::ErrorInLine(const QString &err, int fileId, int lineId)
  {
  SvError error;
  error.mError = err;
  error.mFile  = mFileTable.value(fileId);
  error.mLine  = lineId;
  mErrorList.append( error );
  }




void SvCompiler::errorInLine(const QString &err, const SvMark &mark)
  {
  ErrorInLine( err, mark.mFile, mark.mLine );
  }






/*
Функция "Ошибка и подавить до конца строки"
Описание
  Выдаем ошибку, подавляем до конца строки
*/
void
SvCompiler::ErrorKillLine( const QString &err ) {
  //Выдать ошибку
  Error( err );
  //Подавить до конца строки
  mPtr = mLine.size();
  //Прочитать дальше
  Blank();
  }






/*
Функция "Ошибка и подавить до конца оператора"
Описание
  Выдаем ошибку, подавляем до конца оператора
*/
void
SvCompiler::ErrorEndSt( const QString &err ) {
  //Выдать ошибку
  Error( err );
  //Подавить до конца оператора
  while( !mEof ) {
    if( !IsEoln() ) {
      if( mLine.at(mPtr++) == QChar(';') ) {
        NextToken();
        return;
        }
      }
    else Blank();
    }
  }


/*
Функция "Выдать список ошибок в виде сплошного текста, разделенного на строки"
Описание
  Для каждой ошибки формируем строку вида "описание ошибки" в файле ff в строке nn
  */
QString
SvCompiler::errorList() {
  QString dest;

  foreach( const SvError &err, mErrorList ) {
    //Вывести сообщение о имени файла и номере ошибки
    dest.append( err.mError ).append( QObject::tr("\nIn file '%1' line %2:\n").arg(err.mFile).arg(err.mLine) );
    }

  return dest;
  }




void
SvCompiler::ErrNeedLValue() {
  Error( QObject::tr("Error. Expression must be LValue.") );
  }









void
SvCompiler::ErrIllegalOperation() {
  Error( QObject::tr("Error. Invalid operation with that types.") );
  }




bool SvCompiler::needToken(int token, const QString need)
  {
  if( mToken == token ) {
    NextToken();
    return true;
    }
  ErrorEndSt( QObject::tr( "Need %1").arg(need) );
  NextToken();
  return false;
  }

