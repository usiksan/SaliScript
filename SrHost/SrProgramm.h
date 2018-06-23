/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef SVPROGRAMM_H
#define SVPROGRAMM_H

#include <QByteArray>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QDataStream>

#include "SrVMachine/SrVmTypes.h"
#include "SrVMachine/SrVmUtils.h"
#include "SrVMachine/SrVmCodeHeader.h"


//Элемент ошибки
struct SrError {
    qint32  mLine;  //Номер строки с ошибкой
    qint32  mColon; //Номер позиции с ошибкой
    QString mFile;  //Имя файла с ошибкой
    QString mError; //Текст ошибки

    SrError() : mLine(0), mColon(0) {}
    SrError( const SrError &src ) { copy(src); }

    SrError& operator = ( const SrError &src ) { copy(src); return *this; }

    void copy( const SrError &src ) { mLine = src.mLine; mColon = src.mColon; mFile = src.mFile; mError = src.mError; }
  };

QDataStream& operator << ( QDataStream &os, const SrError &err );
QDataStream& operator >> ( QDataStream &is, SrError &err );

//Список ошибок
typedef QList<SrError> SvErrorList;





//Ссылка на место в файле исходного текста
struct SrMark {
    qint32 mFile;   //Номер файла
    qint32 mLine;   //Номер строки
    qint32 mColon;  //Номер столбца

    //Построить пустой маркер
    SrMark() : mFile(0), mLine(0), mColon(0) {}

    //Построить маркер из отдельных частей
    SrMark( int file, int line, int colon ) : mFile(file), mLine(line), mColon(colon) {}

    //Построить копию
    SrMark( const SrMark &src ) : mFile(src.mFile), mLine(src.mLine), mColon(src.mColon) {}


    void clear() { mFile = 0; mLine = 0; mColon = 0; }
  };

typedef QList<SrMark> SrMarkList;







class SrProgramm
  {
    QString             mName;        //!< Наименование узла
    qint32              mVariant;     //!< Вариант контроллера для данного сегмента
    qint32              mVersion;     //!< Версия ПО для данного сегмента
    QByteArray          mCode;        //!< Массив, содержащий байт-код
    QList<qint32>       mLines;       //!< Массив, содержащий перечень строк, соответствующих адресу в байт-коде
    QList<qint32>       mFiles;       //!< Массив, содержащий перечень файлов, соответствующих адресу в байт-коде
    QMap<QString,int>   mSymbols;     //!< Пары Имя-значение (адрес)


    //Вспомогательные таблицы (не сохраняются)
  public:
    QStringList         mStrings;     //!< Строки
    SvErrorList         mErrors;      //!< Список ошибок
    QStringList         mFileList;    //!< Список исходных файлов

    SrProgramm();
    SrProgramm( QByteArray &src );
    SrProgramm( int variant, int version ) : mVariant(variant), mVersion(version) { clear(); }

    ~SrProgramm();

    //Получить список ошибок как одну строку
    QString errorList() const;

    void setName( const QString name ) { mName = name; }
    void setVariant( int variant ) { mVariant = variant; }
    void setVersion( int version ) { mVersion = version; }

    //Хэш значение программы
    void setHashValue();

    //Установить сигнатуру
    void setSignature( const QString name );

    //Таблица инициализации
    //Каждая запись таблицы адрес 3 байта : значение 4 байта
    //Последняя запись - нуль
    void setInitTable();

    //Точка входа
    void setEntry( int start );

    //Размер стека для начальной задачи
    void setStackSize0( int size );

    //Размер программы VPU (только код)
    void setVpuProgSize();

    //Количество глобальных переменных
    void setGlobalCount( int c );

    //Таблица константных блоков
    void setConstTable();

    //Размер всей программы
    void setProgSize();

    void clear();


    //Основные операции добавления
    //! Добавить код в таблицу кода. Один байт.
    void add8( int value, int fileIndex, int lineIndex );

    //! Добавить 16-битное значение
    void add16( int value, int fileIndex, int lineIndex );

    //! Добавить 24-битное значение
    void add24( int value, int fileIndex, int lineIndex );

    //! Добавить 32-битное значение
    void add32( int value, int fileIndex, int lineIndex );

    //! Добавить символ в таблицу
    void addSymbol(const QString sym, int addr);

    //! Разместить 32-битное значение
    void set32( int addr, int value );




    //Вспомогательные операции
    //! Добавить код в таблицу кода. Один байт.
    void addCode(int code, const SrMark &mark );

    //! Добавить код в таблицу кода. Два байта (байт кода и байт параметра)
    void addCodeParam8(int code, int param, const SrMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два параметра по одному байту)
    void addCode2Param8(int code, int param1, int param2, const SrMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
    void addCodeParam16(int code, int param, const SrMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
    void addCodeParam24(int code, int param, const SrMark &mark);

    //! Добавить код в таблицу кода. Пять байт (байт кода и четыре байта параметра)
    void addCodeParam32( int code, int param, const SrMark &mark );

    //! Создание свойства
    void addProperty(int code, int bindingAddress, int sceneIndex, int init, const SrMark &mark);

    //! Создание сцены
    void addScene( int sceneId, int propertyCount, int childCount, const SrMark &mark );




    //Операции извлечения
    QByteArray getCode() { return mCode; }
    SrVmCode*  getVpuCode() const { return (SrVmCode*)(mCode.data()); }
    int        getCode( int addr ) { return mCode[addr]; }
    int        getLine( int addr ) { return mLines[addr]; }
    int        getFile( int addr ) { return mFiles[addr]; }
    int        getAddr( const QString sym );
    QString    getSymbol( int addr );
    QString    getName() const { return mName; }
    int        codeCount() const { return mCode.count(); }
    int        getVariant() const { return mVariant; }
    int        getVersion() const { return mVersion; }
    QString    getFileByIndex( int index );

    //===========================
    //Информация из программы
    //===========================
    //Информация из программы
    //Хэш значение программы
    int  hashProg()    const { return svIRead32( getVpuCode() + SRVMH_HASH ); }
    //Сигнатура (имя программы) Текстовое поле длиной 16 байт
    int  signature0()  const { return svIRead32( getVpuCode() + SRVMH_SIGNATURE ); }
    int  signature1()  const { return svIRead32( getVpuCode() + SRVMH_SIGNATURE + 4 ); }
    int  signature2()  const { return svIRead32( getVpuCode() + SRVMH_SIGNATURE + 8 ); }
    int  signature3()  const { return svIRead32( getVpuCode() + SRVMH_SIGNATURE + 12 ); }
    //Таблица инициализации
    //Каждая запись таблицы адрес 3 байта : значение 4 байта
    //Последняя запись - нуль
    int  initTable()   const { return svIRead32( getVpuCode() + SRVMH_INIT_TABLE ); }
    //Точка входа
    int  enterPoint()  const { return svIRead32( getVpuCode() + SRVMH_ENTER_POINT ); }
    //Размер стека для начальной задачи
    int  stackSize0()  const { return svIRead32( getVpuCode() + SRVMH_STACK_SIZE0 ); }
    //Размер программы VPU (только код)
    int  vpuProgSize() const { return svIRead32( getVpuCode() + SRVMH_VM_PROG_SIZE ); }
    //Количество глобальных переменных
    int  globalCount() const { return svIRead32( getVpuCode() + SRVMH_GLOBAL_COUNT ); }
    //Таблица строк
    int  constTable()  const { return svIRead32( getVpuCode() + SRVMH_CONST_TABLE ); }
    //Размер всей программы
    int  progSize()    const { return svIRead32( getVpuCode() + SRVMH_PROG_SIZE ); }



    //Запись-чтение
    void       write( QDataStream &os );
    void       read( QDataStream &is );

    QByteArray toArray();
    void       save( const QString &fname );
    void       load( const QString &fname );
  };


typedef QSharedPointer<SrProgramm> SrProgrammPtr;


#endif // SVPROGRAMM_H
