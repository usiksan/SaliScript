/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SrProgramm.h"
#include <QFile>
#include <QDebug>

QDataStream& operator << ( QDataStream &os, const SrError &err )
  {
  os << err.mLine
     << err.mColon
     << err.mFile
     << err.mError;
  return os;
  }

QDataStream& operator >> ( QDataStream &is, SrError &err )
  {
  is >> err.mLine
     >> err.mColon
     >> err.mFile
     >> err.mError;
  return is;
  }





SrProgramm::SrProgramm()
  {
  clear();
  }




SrProgramm::SrProgramm(QByteArray &src)
  {
  QDataStream is(src);
  read( is );
  }




SrProgramm::~SrProgramm()
  {
  clear();
  }




//Получить список ошибок как одну строку
QString SrProgramm::errorList() const
  {
  QString dest;

  foreach( const SrError &err, mErrors ) {
    //Вывести сообщение о имени файла и номере ошибки
    dest.append( err.mError ).append( QObject::tr("\nIn file '%1' line %2:\n").arg(err.mFile).arg(err.mLine) );
    }

  return dest;
  }




int SrProgramm::getAddr( const QString symbol )
  {
  return mSymbols.value( symbol, 0 );
  }










//Хэш значение программы
void SrProgramm::setHashValue()
  {
  //Записываем хэш-значение программы в первые 4 байта
  //Значение считаем по всей программе без учета первых 4-х байтов
  set32( SRVMH_HASH, qHashBits( mCode.data() + 4, mCode.count() - 4 ) );
  }








QByteArray SrProgramm::toArray()
  {
  QByteArray array;
  QDataStream os(&array,QIODevice::WriteOnly);
  write( os );
  return array;
  }




void SrProgramm::save(const QString &fname)
  {
  //Образовать файл
  QFile file(fname);
  if( file.open(QIODevice::WriteOnly) ) {
    //Образовать поток
    QDataStream os( &file );
    //Записать
    write( os );
    }
  }






//! Разместить сигнатуру
void SrProgramm::setSignature(const QString name)
  {
  QByteArray latin = name.toLatin1();
  for( int i = 0; i < SRVMH_SIGNATURE_LENGHT && i < latin.count(); i++ )
    mCode[i] = latin[i];
  }




void SrProgramm::setInitTable()
  {
  set32( SRVMH_INIT_TABLE, mCode.count() );
  }




void SrProgramm::setEntry(int start)
  {
  set32( SRVMH_ENTER_POINT, start );
  }



//Размер стека для начальной задачи
void SrProgramm::setStackSize0(int size)
  {
  set32( SRVMH_STACK_SIZE0, size );
  }





//Размер программы VPU (только код)
void SrProgramm::setVpuProgSize()
  {
  set32( SRVMH_VM_PROG_SIZE, mCode.count() );
  }




void SrProgramm::setProgSize()
  {
  set32( SRVMH_PROG_SIZE, mCode.count() );
  }



void SrProgramm::setGlobalCount(int c)
  {
  set32( SRVMH_GLOBAL_COUNT, c );
  }



//Таблица константных блоков
void SrProgramm::setConstTable()
  {
  set32( SRVMH_CONST_TABLE, mCode.count() );
  }









void SrProgramm::clear()
  {
  mCode.clear();
  mLines.clear();
  mFiles.clear();
  mSymbols.clear();
  mErrors.clear();

  //Оставить место под заголовок
  for( int i = 0; i < SRVMH_HEADER_SIZE; i++ )
    add8( 0, 0, 0 );
  }




void SrProgramm::add8(int value, int fileIndex, int lineIndex)
  {
  mCode.append( (char)value );
  mLines.append( lineIndex );
  mFiles.append( fileIndex );
  }




//! Добавить 16-битное значение
void SrProgramm::add16(int value, int fileIndex, int lineIndex)
  {
  add8( value >> 8, fileIndex, lineIndex );
  add8( value, fileIndex, lineIndex );
  }




//! Добавить 24-битное значение
void SrProgramm::add24(int value, int fileIndex, int lineIndex)
  {
  add8( value >> 16, fileIndex, lineIndex );
  add8( value >> 8, fileIndex, lineIndex );
  add8( value, fileIndex, lineIndex );
  }





//! Добавить 32-битное значение
void SrProgramm::add32(int value, int fileIndex, int lineIndex)
  {
  add16( value >> 16, fileIndex, lineIndex );
  add16( value, fileIndex, lineIndex );
  }





//! Добавить символ в таблицу
void SrProgramm::addSymbol(const QString sym, int addr )
  {
  mSymbols.insert( sym, addr );
  }





void SrProgramm::set32(int addr, int value)
  {
  mCode[addr+0] = value >> 24;
  mCode[addr+1] = value >> 16;
  mCode[addr+2] = value >> 8;
  mCode[addr+3] = value;
  }






//! Добавить код в таблицу кода. Один байт.
void SrProgramm::addCode(int code, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Два байта (байт кода и байт параметра)
void SrProgramm::addCodeParam8(int code, int param, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add8( param, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Три байта (байт кода и два параметра по одному байту)
void SrProgramm::addCode2Param8(int code, int param1, int param2, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add8( param1, mark.mFile, mark.mLine );
  add8( param2, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
void SrProgramm::addCodeParam16(int code, int param, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add16( param, mark.mFile, mark.mLine );
  }



void SrProgramm::addCodeParam24(int code, int param, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add24( param, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Пять байт (байт кода и четыре байта параметра)
void SrProgramm::addCodeParam32(int code, int param, const SrMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add32( param, mark.mFile, mark.mLine );
  }








QString SrProgramm::getSymbol(int addr)
  {
  return mSymbols.key( addr, QString() );
  }




QString SrProgramm::getFileByIndex(int index)
  {
  if( index >= 0 && index < mFileList.count() )
    return mFileList.at(index);
  return QString();
  }



void SrProgramm::write(QDataStream &os)
  {
  os << mVariant     //!< Вариант контроллера для данного сегмента
     << mVersion     //!< Версия ПО для данного сегмента
     << mCode        //!< Массив, содержащий байт-код
     << mLines       //!< Массив, содержащий перечень строк, соответствующих адресу в байт-коде
     << mFiles       //!< Массив, содержащий перечень файлов, соответствующих адресу в байт-коде
     << mSymbols     //!< Пары Имя-значение (адрес)
     << mStrings     //!< Строки
     << mErrors;     //!< Список ошибок
  }

void SrProgramm::read(QDataStream &is)
  {
  is >> mVariant     //!< Вариант контроллера для данного сегмента
     >> mVersion     //!< Версия ПО для данного сегмента
     >> mCode        //!< Массив, содержащий байт-код
     >> mLines       //!< Массив, содержащий перечень строк, соответствующих адресу в байт-коде
     >> mFiles       //!< Массив, содержащий перечень файлов, соответствующих адресу в байт-коде
     >> mSymbols     //!< Пары Имя-значение (адрес)
     >> mStrings     //!< Строки
     >> mErrors;     //!< Список ошибок
  }




