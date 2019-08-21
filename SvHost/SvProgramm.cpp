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
    Реализация класса SvProgramm - программа на байт-коде
*/
#include "SvProgramm.h"
#include <QFile>
#include <QDebug>

QDataStream& operator << ( QDataStream &os, const SvError &err )
  {
  os << err.mLine
     << err.mColon
     << err.mFile
     << err.mError;
  return os;
  }

QDataStream& operator >> ( QDataStream &is, SvError &err )
  {
  is >> err.mLine
     >> err.mColon
     >> err.mFile
     >> err.mError;
  return is;
  }





SvProgramm::SvProgramm()
  {
  clear();
  }




SvProgramm::SvProgramm(QByteArray &src)
  {
  QDataStream is(src);
  read( is );
  }




SvProgramm::~SvProgramm()
  {
  clear();
  }




//Получить список ошибок как одну строку
QString SvProgramm::errorList() const
  {
  QString dest;

  foreach( const SvError &err, mErrors ) {
    //Вывести сообщение о имени файла и номере ошибки
    dest.append( err.mError ).append( QObject::tr("\nIn file '%1' line %2:\n").arg(err.mFile).arg(err.mLine) );
    }

  return dest;
  }




int SvProgramm::getAddr( const QString symbol )
  {
  return mSymbols.value( symbol, 0 );
  }










//Хэш значение программы
void SvProgramm::setHashValue()
  {
  //Записываем хэш-значение программы в первые 4 байта
  //Значение считаем по всей программе без учета первых 4-х байтов
  set32( SVVMH_HASH, static_cast<int>(qHashBits( mCode.data() + 4, static_cast<size_t>(mCode.count() - 4) ))  );
  }








QByteArray SvProgramm::toArray()
  {
  QByteArray array;
  QDataStream os(&array,QIODevice::WriteOnly);
  write( os );
  return array;
  }




void SvProgramm::save(const QString &fname)
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
void SvProgramm::setSignature(const QString name)
  {
  QByteArray latin = name.toLatin1();
  for( int i = 0; i < SVVMH_SIGNATURE_LENGHT && i < latin.count(); i++ )
    mCode[i] = latin[i];
  }




void SvProgramm::setInitTable()
  {
  set32( SVVMH_INIT_TABLE, mCode.count() );
  }




void SvProgramm::setEntry(int start)
  {
  set32( SVVMH_ENTER_POINT, start );
  }



//Размер стека для начальной задачи
void SvProgramm::setStackSize0(int size)
  {
  set32( SVVMH_STACK_SIZE0, size );
  }





//Размер программы VPU (только код)
void SvProgramm::setVpuProgSize()
  {
  set32( SVVMH_VM_PROG_SIZE, mCode.count() );
  }




void SvProgramm::setProgSize()
  {
  set32( SVVMH_PROG_SIZE, mCode.count() );
  }



void SvProgramm::setGlobalCount(int c)
  {
  set32( SVVMH_GLOBAL_COUNT, c );
  }



//Таблица константных блоков
void SvProgramm::setConstTable()
  {
  set32( SVVMH_CONST_TABLE, mCode.count() );
  }









void SvProgramm::clear()
  {
  mCode.clear();
  mLines.clear();
  mFiles.clear();
  mSymbols.clear();
  mErrors.clear();

  //Оставить место под заголовок
  for( int i = 0; i < SVVMH_HEADER_SIZE; i++ )
    add8( 0, 0, 0 );
  }




void SvProgramm::add8(int value, int fileIndex, int lineIndex)
  {
  mCode.append( (char)value );
  mLines.append( lineIndex );
  mFiles.append( fileIndex );
  }




//! Добавить 16-битное значение
void SvProgramm::add16(int value, int fileIndex, int lineIndex)
  {
  add8( value >> 8, fileIndex, lineIndex );
  add8( value, fileIndex, lineIndex );
  }




//! Добавить 24-битное значение
void SvProgramm::add24(int value, int fileIndex, int lineIndex)
  {
  add8( value >> 16, fileIndex, lineIndex );
  add8( value >> 8, fileIndex, lineIndex );
  add8( value, fileIndex, lineIndex );
  }





//! Добавить 32-битное значение
void SvProgramm::add32(int value, int fileIndex, int lineIndex)
  {
  add16( value >> 16, fileIndex, lineIndex );
  add16( value, fileIndex, lineIndex );
  }





//! Добавить символ в таблицу
void SvProgramm::addSymbol(const QString sym, int addr )
  {
  mSymbols.insert( sym, addr );
  }





void SvProgramm::set32(int addr, int value)
  {
  mCode[addr+0] = value >> 24;
  mCode[addr+1] = value >> 16;
  mCode[addr+2] = value >> 8;
  mCode[addr+3] = value;
  }






//! Добавить код в таблицу кода. Один байт.
void SvProgramm::addCode(int code, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Два байта (байт кода и байт параметра)
void SvProgramm::addCodeParam8(int code, int param, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add8( param, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Три байта (байт кода и два параметра по одному байту)
void SvProgramm::addCode2Param8(int code, int param1, int param2, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add8( param1, mark.mFile, mark.mLine );
  add8( param2, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
void SvProgramm::addCodeParam16(int code, int param, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add16( param, mark.mFile, mark.mLine );
  }



void SvProgramm::addCodeParam24(int code, int param, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add24( param, mark.mFile, mark.mLine );
  }




//! Добавить код в таблицу кода. Пять байт (байт кода и четыре байта параметра)
void SvProgramm::addCodeParam32(int code, int param, const SvMark &mark )
  {
  add8( code, mark.mFile, mark.mLine );
  add32( param, mark.mFile, mark.mLine );
  }








QString SvProgramm::getSymbol(int addr)
  {
  return mSymbols.key( addr, QString() );
  }




QString SvProgramm::getFileByIndex(int index)
  {
  if( index >= 0 && index < mFileList.count() )
    return mFileList.at(index);
  return QString();
  }



void SvProgramm::write(QDataStream &os)
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

void SvProgramm::read(QDataStream &is)
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




