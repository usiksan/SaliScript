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
    Программа скриптового языка rc++
*/

#ifndef SVPROGRAMM_H
#define SVPROGRAMM_H

#include <QByteArray>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QDataStream>

#include "SvVMachine/SvVmTypes.h"
#include "SvVMachine/SvVmUtils.h"
#include "SvVMachine/SvVmCodeHeader.h"

//Расширение для бинарного представления программы
#define SV_PROGRAMM_EXTENSION QString(".vpu6prg")

//Элемент ошибки
struct SvError {
    qint32  mLine;  //Номер строки с ошибкой
    qint32  mColon; //Номер позиции с ошибкой
    QString mFile;  //Имя файла с ошибкой
    QString mError; //Текст ошибки

    SvError() : mLine(0), mColon(0) {}
    SvError( const SvError &src ) { copy(src); }

    SvError& operator = ( const SvError &src ) { copy(src); return *this; }

    void copy( const SvError &src ) { mLine = src.mLine; mColon = src.mColon; mFile = src.mFile; mError = src.mError; }
  };

QDataStream& operator << ( QDataStream &os, const SvError &err );
QDataStream& operator >> ( QDataStream &is, SvError &err );

//Список ошибок
typedef QList<SvError> SvErrorList;





//Ссылка на место в файле исходного текста
struct SvMark {
    qint32 mFile;   //Номер файла
    qint32 mLine;   //Номер строки
    qint32 mColon;  //Номер столбца

    //Построить пустой маркер
    SvMark() : mFile(0), mLine(0), mColon(0) {}

    //Построить маркер из отдельных частей
    SvMark( int file, int line, int colon ) : mFile(file), mLine(line), mColon(colon) {}

    //Построить копию
    SvMark( const SvMark &src ) : mFile(src.mFile), mLine(src.mLine), mColon(src.mColon) {}


    void clear() { mFile = 0; mLine = 0; mColon = 0; }
  };

typedef QList<SvMark> SvMarkList;







class SvProgramm
  {
    qint32              mVariant;     //!< Вариант контроллера
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

    SvProgramm();
    SvProgramm( QByteArray &src );
    SvProgramm( int variant, int version ) : mVariant(variant), mVersion(version) { clear(); }

    ~SvProgramm();


    /*!
       \brief errorList Возвращает список ошибок как одну строку
       \return Список ошибок в виде одной строки

       Список ошибок хранится в программе в качетсве списка специальных структур.
       В каждой такой структуре указано место нахождения ошибки и описание ошибки.
       Структура с ошибками позволяет осуществить навигацию к месту возникновения
       ошибки. Для того, чтобы просто вывести список ошибок используется данная
       функция, которая собирает все описания ошибок и возвращает их как единый текст.
     */
    QString    errorList() const;





    /*!
       \brief setVariant Установить вариант виртуальной машины, для которой предназначен данный скрипт
       \param variant Вариант виртуальной машины

       Хотя виртуальные машины одинаковы по байт-коду, они отличаются набором встроенных
       функций. Для того, чтобы убедиться, что данный скрипт будет работать на указанной машине
       и используется данный код.
     */
    void       setVariant( int variant ) { mVariant = variant; }

    /*!
       \brief getVariant Получить вариант контроллера
       \return Вариант контроллера
     */
    int        getVariant() const { return mVariant; }




    /*!
       \brief setVersion Устанавливает версию ПО виртуальной машины, для которой предназначен данный скрипт.
       \param version Версия ПО виртуальной машины.

       Одинаковые виртуальные машины могут различаться версией ПО, при которых вводятся изменения в
       интерфейс встроенных функций, изменяется обработка и т.п. Для того, чтобы убедиться, что
       данный скрипт будет работать на виртуальной машине с нужной версией ПО
     */
    void       setVersion( int version ) { mVersion = version; }

    /*!
       \brief getVersion Возвращает версию ПО виртуальной машины, для которой предназначен данный скрипт.
       \return Версия ПО виртуальной машины
     */
    int        getVersion() const { return mVersion; }





    /*!
       \brief setHashValue Вычисляет и устанавливает хэш значение кода программы.

       Для того, чтобы исключить ненужные перепрошивки программа установления связи
       с контроллером может сначала сравнить хэш-значение программ и осуществить
       перепрошивку только если они различаются.
     */
    void       setHashValue();

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
    void addCode(int code, const SvMark &mark );

    //! Добавить код в таблицу кода. Два байта (байт кода и байт параметра)
    void addCodeParam8(int code, int param, const SvMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два параметра по одному байту)
    void addCode2Param8(int code, int param1, int param2, const SvMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
    void addCodeParam16(int code, int param, const SvMark &mark);

    //! Добавить код в таблицу кода. Три байта (байт кода и два байта параметра)
    void addCodeParam24(int code, int param, const SvMark &mark);

    //! Добавить код в таблицу кода. Пять байт (байт кода и четыре байта параметра)
    void addCodeParam32( int code, int param, const SvMark &mark );

    //! Создание свойства
    void addProperty(int code, int bindingAddress, int sceneIndex, int init, const SvMark &mark);

    //! Создание сцены
    void addScene( int sceneId, int propertyCount, int childCount, const SvMark &mark );




    //Операции извлечения
    QByteArray getCode() { return mCode; }
    SvVmCode*  getVpuCode() const { return static_cast<const SvVmCode*>( static_cast<const void*>(mCode.data()) ); }
    int        getCode( int addr ) { return mCode[addr]; }
    int        getLine( int addr ) { return mLines[addr]; }
    int        getFile( int addr ) { return mFiles[addr]; }
    int        getAddr( const QString sym );
    QString    getSymbol( int addr );
    int        codeCount() const { return mCode.count(); }
    QString    getFileByIndex( int index );





    //===========================
    //Информация из программы
    //===========================
    //Информация из программы

    /*!
       \brief hashProg Возвращает хэш значение текущей программы
       \return Хэш значение текущей программы
     */
    int  hashProg()    const { return svIRead32( getVpuCode() + SVVMH_HASH ); }

    /*!
       \brief signature0 Возвращает сигнатуру программы как целое
       \return 4 байта сигнатуры программы

       Группа функций возвращает сигнатуру программы как набор целых.
       Все вместе они образуют текстовую строку, представляющую собой сигнатуру
       программы - текстовое имя.
     */
    int  signature0()  const { return svIRead32( getVpuCode() + SVVMH_SIGNATURE ); }
    int  signature1()  const { return svIRead32( getVpuCode() + SVVMH_SIGNATURE + 4 ); }
    int  signature2()  const { return svIRead32( getVpuCode() + SVVMH_SIGNATURE + 8 ); }
    int  signature3()  const { return svIRead32( getVpuCode() + SVVMH_SIGNATURE + 12 ); }
    int  signature4()  const { return svIRead32( getVpuCode() + SVVMH_SIGNATURE + 16 ); }


    /*!
       \brief initTable Возвращает начало таблицы инициализации
       \return Начало таблицы инициализации

       Таблица инициализации состоит из записей размером 7 байт.
       Каждая запись таблицы адрес 3 байта : значение 4 байта
       Последняя запись - нуль
     */
    int  initTable()   const { return svIRead32( getVpuCode() + SVVMH_INIT_TABLE ); }




    /*!
       \brief enterPoint Адрес точки входа в программу. Это адрес функции main
       \return Адрес функции main
     */
    int  enterPoint()  const { return svIRead32( getVpuCode() + SVVMH_ENTER_POINT ); }




    /*!
       \brief stackSize0 Возвращает размер стека для стартовой задачи
       \return Размер стека для начальной задачи
     */
    int  stackSize0()  const { return svIRead32( getVpuCode() + SVVMH_STACK_SIZE0 ); }



    /*!
       \brief vpuProgSize Возвращает размер чистого кода программы без учета последующих
         таблиц
       \return Размер чистого кода
     */
    int  vpuProgSize() const { return svIRead32( getVpuCode() + SVVMH_VM_PROG_SIZE ); }



    /*!
       \brief globalCount Возвращает объем памяти, занятый глобальными переменными
       \return Объем памяти, занятый глобальными переменными
     */
    int  globalCount() const { return svIRead32( getVpuCode() + SVVMH_GLOBAL_COUNT ); }




    /*!
       \brief constTable Возвращает начало таблицы константных блоков переменной длины.
       \return Начало таблицы константных блоков

       Каждая запись таблицы состоит из адреса начала блока. Длина записи 4 байта.
       Размер блока определяется в самом блоке. Константными блоками представлены
       изображения, звуки, текстовые строки и т.п.
     */
    int  constTable()  const { return svIRead32( getVpuCode() + SVVMH_CONST_TABLE ); }



    /*!
       \brief progSize Возвращает размер памяти программ включая все дополнительные таблицы
       \return Размер всей программы
     */
    int  progSize()    const { return svIRead32( getVpuCode() + SVVMH_PROG_SIZE ); }



    //Запись-чтение
    void       write( QDataStream &os );
    void       read( QDataStream &is );

    QByteArray toArray();
    void       save( const QString &fname );
    void       load( const QString &fname );
  };


typedef QSharedPointer<SvProgramm> SvProgrammPtr;


#endif // SVPROGRAMM_H
