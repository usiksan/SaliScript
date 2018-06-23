/*
  Project     "Script language reduced c++ (rc++)"
  SubProject  "Virtual machine"
  Author
    Alexander Sibilev
  Internet
    www.rc.saliLab.ru - project home site
    www.saliLab.ru
    www.saliLab.com

  Description
    reduced c++ (rc++) pico compiler
*/
#ifndef SRCOMPILER_H
#define SRCOMPILER_H

#include "SrHost/SrProgramm.h"

#include <QStack>
#include <QHash>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QDebug>

#define TVERSION "V6.05"
#define Assert(x)
#define TFUN_FRAME_SIZE     3  //Размер фрейма функции
#define SV_OPER_MAX        32  //Максимальное количество операндов в одной бинарной операции
#define SV_FUN_PARAM_MAX   32  //Максимальное количество параметров в вызове функции

namespace SrCompiler6
  {
  //Определение типов для внутреннего использования компилятором
  typedef unsigned short SvChar;


  //Коды Token сканера
  enum {
    ttEof,         //Конец потока

    //Ключевые слова
    tkwAt,         //_at_       !!!
    tkwAnd,        //and
    tkwAndEq,      //and_eq
    tkwAsm,        //asm
    tkwAuto,       //auto
    tkwBool,       //bool
    tkwBreak,      //break
    tkwCase,       //case
    tkwCatch,      //catch
    tkwCblock,     //cblock     !!!
    tkwChar,       //char
    tkwClass,      //class
    tkwConst,      //const
    tkwConstCast,  //const_cast
    tkwContinue,   //continue
    tkwCString,    //cstring
    tkwDefault,    //default
    tkwDelete,     //delete
    tkwDo,         //do
    tkwDouble,     //double
    tkwDynamicCast,//dynamic_cast
    tkwElse,       //else
    tkwEnum,       //enum
    tkwExplicit,   //explicit
    tkwExport,     //export
    tkwExtern,     //extern
    tkwFalse,      //false
    tkwFloat,      //float
    tkwFor,        //for
    tkwFriend,     //friend
    tkwFunction,   //function
    tkwGoto,       //goto
    tkwIf,         //if
    tkwImport,     //import       !!!
    tkwInline,     //inline
    tkwInt,        //int
    tkwLong,       //long
    tkwMutable,    //mutable
    tkwNamespace,  //namespace
    tkwNew,        //new
    tkwOperator,   //operator
    tkwPrivate,    //private
    tkwProperty,   //property
    tkwProtected,  //protected
    tkwPublic,     //public
    tkwRegister,   //register
    tkwReinterpretCast, //reinterpret_cast
    tkwReturn,     //return
    tkwShort,      //short
    tkwSigned,     //signed
    tkwSizeof,     //sizeof
    tkwStatic,     //static
    tkwStaticCast, //static_cast
    tkwStruct,     //struct
    tkwSwitch,     //switch
    tkwTemplate,   //template
    tkwThis,       //this
    tkwThrow,      //throw
    tkwTrue,       //true
    tkwTry,        //try
    tkwTypedef,    //typedef
    tkwTypeid,     //typeid
    tkwTypename,   //typename
    tkwUnion,      //union
    tkwUnsigned,   //unsigned
    tkwUsing,      //using
    tkwVint,       //vint
    tkwVirtual,    //virtual
    tkwVoid,       //void
    tkwVolatile,   //volatile
    tkwWchar_t,    //wchar_t
    tkwWhile,      //while

    //Знаки
    tsFirst,
    tsFOpen,       // {
    tsFClose,      // }
    tsSOpen,       // [
    tsSClose,      // ]
    tsMatrix,      // #
    tsDMatrix,     // ##
    tsOpen,        // (
    tsClose,       // )
    tsCLeft,       // <:
    tsCRight,      // :>
    tsPLeft,       // <%
    tsPRight,      // %>
    tsPCPC,        // %:%:
    tsPC,          // %:
    tsSemicolon,   // ;
    tsColon,       // :
    tsQuestion,    // ?
    tsDColon,      // ::
    tsPoint,       // .
    tsPointMul,    // .*
    tsPlus,        // +
    tsMinus,       // -
    tsMul,         // *
    tsDiv,         // /
    tsMod,         // %
    tsXor,         // ^
    tsAnd,         // &
    tsOr,          // |
    tsNot,         // ~
    tsLNot,        // !
    tsAssign,      // =
    tsLess,        // <
    tsGrow,        // >
    tsPlusAsg,     // +=
    tsMinusAsg,    // -=
    tsMulAsg,      // *=
    tsDivAsg,      // /=
    tsModAsg,      // %=
    tsXorAsg,      // ^=
    tsAndAsg,      // &=
    tsOrAsg,       // |=
    tsLShiftAsg,   // <<=
    tsRShiftAsg,   // >>=
    tsLShift,      // <<
    tsRShift,      // >>
    tsEqu,         // ==
    tsNotEqu,      // !=
    tsLessEqu,     // <=
    tsGrowEqu,     // >=
    tsLAnd,        // &&
    tsLOr,         // ||
    tsInc,         // ++
    tsDec,         // --
    tsComma,       // ,
    tsRef,         // ->
    tsRefMul,      // ->*
    tsMultiPoint,  // ...

    //Остатки
    ttName,        // символьное имя
    ttType,        // тип
    ttScene,       // сцена
    ttInt,
    ttFloat,
    ttString,
    ttUndefined,   // неопределенный код

    ttLast };

  //Основные типы данных
  enum {
    TTYPE_VOID         = 0x000001,
    TTYPE_INT          = 0x000002,   //Целое 32 бит
    TTYPE_FLOAT        = 0x000004,   //Floating point 32 bit wide
    TTYPE_FUNCTION     = 0x000008,   //Функция
    TTYPE_STRUCT       = 0x000010,   //Структура
    TTYPE_CSTRING      = 0x000020,   //Константная строка
    TTYPE_CBLOCK       = 0x000040,   //Блок константных данных (картинка, звук и т.п.)
    TTYPE_POINTER      = 0x000080,   //Указатель на что-нибудь
    TTYPE_ARRAY        = 0x000100,   //Array of items
    TTYPE_LAST };



  struct SrType;
  typedef SrType *SrTypePtr;





  //Вид переменной
  enum SrSort {
    tsrGlobal,
    tsrLocal,
    tsrMember
    };







  //Ссылка на элемент помощи
  struct SrHelp {
      QString         mBrief;         //Короткое описание
      QString         mReference;     //Ссылка на документацию
      SrMark          mDefinePos;     //Позиция определения

      //Конструктор пустой помощи
      SrHelp();

      SrHelp( const SrHelp &hlp );

      void update( const SrHelp &hlp );

      void clear();
    };



  struct SrMacro {
      SrHelp     mHelp;
      QString    mExpander;      //Macro expanding string
      SrMarkList mReferneceList; //Using ref points list for this macro
    };

  typedef SrMacro *SrMacroPtr;
  typedef QHash<QString,SrMacroPtr> SrMacroPtrTable;


  struct SrValue;
  typedef SrValue *SrValuePtr;
  typedef QList<SrValuePtr> SrValuePtrList;

  struct SrValueList {
      SrValuePtrList mList;

      SrValueList() : mList() {}

      ~SrValueList() { qDeleteAll(mList); }

      int     addExpression( SrValue *val ) { mList.append( val ); return mList.count() - 1; }

      void    clear() { qDeleteAll(mList); mList.clear(); }

      QString listing();
    };






  //Переменная
  struct SrVariable {
      SrHelp           mHelp;               //Помощь по переменной
      QString          mName;               //Имя переменной
      SrType          *mType;               //Тип переменной
      SrSort           mSort;               //Сорт (область) переменной: глобальная, локальная, член
      int              mAddress;            //Адрес начала относительный
      int              mArraySize;          //Размер массива (для массивов)
      SrMark           mMarkDefine;         //Место определения
      QString          mRemark;             //Описание переменной
      SrValue         *mInit;               //Инициализационное значение
      SrMarkList       mReferenceList;      //Using ref points list for this variable


      SrVariable();
      SrVariable( const SrMark &src, const QString name, SrType *type, int addr, int arraySize = 0 );
      SrVariable( const SrVariable *src );

      ~SrVariable();

      //Добавить помощь
      void      setRemark( const QString &prevRemark, const QString &afterRemark );

      //Получить размер переменной
      int       getSize();

      //Листинг
      QString   printListing();
    };

  typedef SrVariable *SrVariablePtr;
  typedef QList<SrVariablePtr> SrVariablePtrList;
  typedef QHash<QString,SrVariablePtr> SrVariablePtrHash;

  //Список переменных
  struct SrVariableList {
      SrVariablePtrList mList; //Последовательный список переменных
      SrVariablePtrHash mHash; //Хэш-таблица быстрого поиска переменных
      SrSort            mSort; //Вид таблицы переменных

      SrVariableList( SrSort sort );
      ~SrVariableList();

      //Очистить список
      void          clear();

      //Добавить уже созданную переменную
      void          addVariable( SrVariable *var );

      //Проверить наличие переменной в списке
      bool          isPresent( const QString name ) const { return mHash.contains( name ); }

      //Добавить в список копию из заданного списка
      void          addList( const SrVariableList &src );

      //Получить переменную
      SrVariable*   getVariable( const QString name ) { return mHash.value( name, 0 ); }

      //Получить листинг переменных
      QString       listing( int level );
    };






  struct SrStruct;
  struct SrOperatorBlock;
  struct SrFunction {
      SrHelp           mHelp;               //Помощь по функции
      QString          mName;               //Имя функции (возможны безымянные функции)
      bool             mDefined;            //Флаг, выставляется, когда тело функции уже определено
      bool             mDeclared;           //Флаг, выставляется, когда функция объявлена (например в классе)
                                            //в глобальном контексте все функции объявлены по умолчанию
      int              mAddress;            //Адрес начала кода
      int              mImportIndex;        //Индекс импортной функции
      int              mParamSize;          //Размер области параметров
      SrMark           mMarkDefine;         //Место определения
      SrMark           mMarkDeclare;        //Место первого объявления
      QString          mRemark;             //Описание переменной
      SrVariableList   mParams;             //Список параметров функции
      SrType          *mResultType;         //Тип результата
      SrType          *mType;               //Тип данной функции
      SrOperatorBlock *mBody;               //Тело функции
      int              mLocalAmount;        //Место под локальные переменные
      SrMarkList       mReferenceList;      //Using reference list for this function

      SrFunction();
      SrFunction( const QString &name, SrType *result, const SrMark &mark );

      int         resultOffset() const;

      //Получить глобальное имя функции
      QString     globalName() const;

      SrVariable* getParam( const QString name ) const { return mParams.mHash.value( name, 0 ); }

      //Добавить параметр, если возможно
      bool        addParam( SrVariable *param );

      //Установить комментарии к функции
      void        setRemark(  const QString &prevRemark, const QString &afterRemark );

      //Сформировать листинг объявления функции
      QString     listingDeclaration();

      //Сформировать листинг определения функции
      QString     listingDefinition();
    };

  typedef SrFunction *SrFunctionPtr;
  typedef QList<SrFunctionPtr> SrFunctionPtrList;
  typedef QHash<QString,SrFunctionPtr> SrFunctionPtrHash;


  struct SrFunctionList {
      SrFunctionPtrList mList; //Список функций контекста
      SrFunctionPtrHash mHash; //Хэш-функций

      SrFunctionList();
      ~SrFunctionList();

      //Очистить список
      void        clear();

      //Добавить функцию к списку
      void        addFunction(SrFunctionPtr fun , const QString name);

      //Заменить функцию в списке
      SrFunction* addFunctionDefinition( SrFunctionPtr fun );

      //Скопировать функции-члены
      void        addMembers(const SrFunctionList &src , SrStruct *svClass );

      //Проверить наличие имени среди функций
      bool        isPresent( const QString name ) const { return mHash.contains(name); }

      //Получить функцию
      SrFunction* getFunction( const QString name ) { return mHash.value( name, 0 ); }

      //Сформировать листинг описаний функций
      QString     listingDeclaration();

      //Сформировать листинг функций с определениями
      QString     listingDefinition();
    };









  class SrStruct;
  class SrFunctionType;
  class SrTypeList;

  //Основной и производный тип TCompiler4
  struct SrType {
      QString            mName;           //Первое имя типа
      QString            mSignature;      //Сигнатура типа
      int                mClass;          //Основной тип (класс)
      int                mNumElem;        //Количество элементов массива
      int                mSize;           //Размер объекта данного типа
      SrType            *mBaseType;       //Базовый тип для производных типов
      SrHelp             mHelp;           //Помощь по типу
      SrTypeList        *mTypeList;       //Список всех типов для генерации новых типов из данного

      //Конструктор по умолчанию
      SrType();

      //Деструктор по умолчанию
      virtual ~SrType();

      //Конструктор типа с именем, классом, размером и подтипом
      SrType(const QString &name, int svClass, int size, SrType *base );

      //Проверить возможность назначения типов
      //т.е. может ли тип src быть присвоен данному типу
      bool            canAssign( SrType *src );

      //Получить id объекта
      int             objectId() const { return (mClass >> 20) & 0xff; }

      //Сконструировать тип-объект
      void            createObject( int i );

      //Получить тип как структуру (класс)
      SrStruct        *toStruct();

      //Получить тип как функцию
      SrFunctionType *toFunction();

      //Получить сигнатуру от указателя на данный тип
      QString         signaturePointer() const;

      //Получить сигнатуру на массив элементов данного типа
      QString         signatureArray() const;

      //Получить указатель на тип
      SrType         *getTypePointer();

      //Получить тип массив
      SrType         *getTypeArray(int numElem);

      bool            isInt()     const { return mClass == TTYPE_INT; }
      bool            isVoid()    const { return mClass == TTYPE_VOID; }
      bool            isArray()   const { return mClass == TTYPE_ARRAY; }
      bool            isPointer() const { return mClass == TTYPE_POINTER; }
      bool            isFloat()   const { return mClass == TTYPE_FLOAT; }
      bool            isCBlock()  const { return mClass == TTYPE_CBLOCK; }
      bool            isCString() const { return mClass == TTYPE_CSTRING; }
      bool            isStruct()  const { return mClass == TTYPE_STRUCT; }

      //Проверить соответствие параметра
      bool            isMatchParam(SrType *src , bool srcNull);

      //Построить сигнатуру
      virtual QString buildSignature();

      //Построить листинг
      virtual QString listing();

    };

  typedef QList<SrTypePtr> SvTypePtrList;
  typedef QHash<QString,SrTypePtr> SvTypePtrHash;
  //typedef QHash<SvTypePtr,QString>

  class SrTypeList {
      SvTypePtrHash mHash; //Типы с именами
      SvTypePtrHash mSign; //Типы с сигнатурами
    public:
      SvTypePtrList mList; //Список всех типов

      SrTypeList();
      ~SrTypeList();

      //Очистить список типов
      void    clear();

      //Проверить наличие типа с заданным именем
      bool    isPresent( const QString &name ) const { return mHash.contains(name); }

      //Получить тип по имени
      SrType* getType( const QString &name ) { return mHash.value( name, 0 ); }

      //Получить тип по сигнатуре
      SrType* getTypeBySignature( const QString &sign ) const { return mSign.value(sign,0); }

      //Получить указатель на тип
      SrType* getTypePointer( SrType *base );

      //Получить тип массив
      SrType* getTypeArray(SrType *base, int numElem);

      //Добавить тип к списку
      SrType* addType( SrType *type );

      //Добавить имя для типа
      void    addTypeName( SrType *type, const QString &name );

      //Сформировать листинг типов
      QString listing();
    };







  //Формальный параметр функции
  struct SrFunParam {
      QString          mName;               //Формальное имя параметра
      SrType          *mType;               //Тип параметра

      SrFunParam() : mName(), mType(0) {}

      SrFunParam( SrVariable *var ) : mName(var->mName), mType(var->mType) {}

      SrFunParam& operator = ( const SrFunParam &p ) { mName = p.mName; mType = p.mType; return *this; }
    };

  typedef QList<SrFunParam> SrParamList;




  //Тип-функция
  struct SrFunctionType : public SrType {
      SrParamList      mParamList;      //Список параметров
      SrType          *mResult;         //Тип результата
      int              mParamSize;      //Размер области параметров

      SrFunctionType() : mParamList(), mResult(0), mParamSize(0) { mClass = TTYPE_FUNCTION; }
      ~SrFunctionType();

      //Построить сигнатуру
      virtual QString buildSignature() override;
    };






  //Тип-класс
  //Тип-внешний объект
  struct SrStruct : public SrType {
      int                mVTable;         //Адрес таблицы виртуальных методов
      //bool              mExtern;
      bool               mDefined;        //Структура определена
      SrStruct          *mBaseStruct;     //Базовый тип
      SrVariableList     mMemberList;     //Список членов

      //Конструктор обычного класса
      SrStruct( const QString name );
      ~SrStruct();

      //Для удобства использования
      bool            isMember( const QString name ) { return mMemberList.mHash.contains(name); }
      SrVariable     *getMember( const QString name ) { return mMemberList.mHash.value(name, 0); }

      void            setBase(SrStruct *base );

      void            addMember( SrVariable *mem );

      void            addFunction( SrFunction *fun );

      //Построить листинг
      virtual QString listing() override;
    };









  //Источник исходного кода
  class SrSource {
    protected:
      int          mLineCount;   //Счетчик строк исходного файла
      int          mFileId;      //Идентификатор файла в общей таблице файлов
      QTextStream *mInputStream; //Исходный код в виде файла
      //QString mFileName;  //Имя исходного файла
    public:

      SrSource( int fileId ) : mLineCount(0), mFileId(fileId), mInputStream(0) {}
      virtual ~SrSource();
      QString ReadLine();
      bool    Eof();
      int     Line() const { return mLineCount; }
      int     FileId() const { return mFileId; }
    };



  //Указатель на источник данных
  typedef SrSource *SrSourcePtr;



  //Источник исходного кода - файл
  class SrFileSource : public SrSource {
      QFile       *mFile;
    public:
      SrFileSource( QFile *file, int fileId );
      ~SrFileSource();
    };



  //Источник исходного кода - строка текста
  class SrStringSource : public SrSource {
      QString mSource;
    public:
      SrStringSource( const QString &src, int fileId );
    };





  enum SrStatement {
    tstNone,
    tstLocal,
    tstIf,
    tstWhile,
    tstReturn,
    tstBreak,
    tstContinue,
    tstExpression,
    tstDo,
    tstFor,
    tstSwitch,
    tstCase,
    tstCatch,
    tstThrow,
    tstBlock,
    tstBinding,
    tstDefault };


  enum SrValueCode {
    svvVariable,        //Переменная (глобальная, локальная, параметр)
    svvFunction,        //Функция (глобальная)
    svvWaitFun,         //Специальная функция wait
    svvMemberVariable,  //Член от произвольной структуры
    svvConstInt,        //Константа int
    svvConstFloat,      //Константа float
    svvConstString,
    svvPointer,
    svvAddress,
    svvPredInc,
    svvPredDec,
    svvPostInc,
    svvPostDec,
    svvNot,
    svvLogNot,
    svvNeg,
    svvStore,
    svvArrayCell,      //Операция индекса массива
    svvMul,
    svvMulStore,
    svvDiv,
    svvDivStore,
    svvMod,
    svvModStore,
    svvAdd,
    svvAddStore,
    svvSub,
    svvSubStore,
    svvLShift,
    svvLShiftStore,
    svvRShift,
    svvRShiftStore,
    svvOrStore,
    svvAndStore,
    svvXorStore,
    svvEqu,
    svvNotEqu,
    svvLessEqu,
    svvLess,
    svvGreat,
    svvGreatEqu,
    svvLogAnd,
    svvLogOr,
    svvOr,
    svvAnd,
    svvXor,
    svvComma,      //Операция ,
    svvCondition,      //Тройная условная операция
    svvCall,      //Вызов функции
    svvLast
    };




  //Базовый элемент промежуточного кода
  struct SrValue {
      SrType      *mType;        //Тип значения, полученного в результате
      SrMark       mMark;        //Место в файле для данного значения
      int          mConstInt;    //Вычисленный результат
      float        mConstFloat;
      bool         mConst;       //Флаг показывает константный результат

      SrValue( const SrMark &mark );
      virtual ~SrValue() {}

      void    clear() { memset( this, 0, sizeof(SrValue) ); }

      //Проверка константного значения
      bool    isConst() const { return mConst; }

      //Преобразовать к значению условия
      bool    toCondition() const { return mConstInt; }

      //Получить целую константу
      int     toConstInt() const { return mConstInt; }

      float   toConstFloat() const { return mConstFloat; }

      //Получить строку листинга
      virtual QString     listing() = 0;

      //Получить код операции
      virtual SrValueCode code() const = 0;

      //Получить тип операции
      virtual SrType*     getType() { return mType; }

      //Получить класс типа результата
              int         getClass() { return getType() == 0 ? 0 : mType->mClass; }
  };




  //Переменная (глобальная, локальная, параметр)
  struct SrValueVariable : SrValue {
      SrVariable *mVariable;

      SrValueVariable( SrVariable *var, const SrMark &mark );

      //Получить строку листинга
      virtual QString     listing() { if( mVariable ) return QString(" <%1> ").arg(mVariable->printListing()); return QString("<ERR>"); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvVariable; }

      //Получить тип операции
      virtual SrType     *getType() override;
    };




  //Функция (глобальная)
  struct SrValueFunction : SrValue {
      SrFunction *mFunction;

      SrValueFunction( SrFunction *fun, const SrMark &mark ) :
        SrValue( mark ), mFunction(fun) { mType = fun->mType; }

      //Получить строку листинга
      virtual QString     listing() override { if( mFunction ) return QString(" <%1> ").arg( mFunction->listingDeclaration() ); return QString("<ERR>"); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvFunction; }

      //Получить тип операции
      virtual SrType     *getType() override { if( mType == 0 && mFunction ) mType = mFunction->mType; return mType; }
    };





  //Специальная функция wait
  struct SrValueWaitFun : SrValue {
      SrValueWaitFun( SrType *type, const SrMark &mark ) : SrValue(mark) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" Wait() "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvWaitFun; }
    };




  //Член от произвольной структуры структуры
  struct SrValueMemberVariable : SrValueVariable {
      SrValue *mStruct; //Выражение вычисления структуры

      SrValueMemberVariable( SrVariable *var, SrValue *struc, const SrMark &mark );
      ~SrValueMemberVariable();

      //Получить строку листинга
      virtual QString     listing() override { if(mStruct) return mStruct->listing() + QString(".") + SrValueVariable::listing(); return QString("ERR.") + SrValueVariable::listing(); }

      //Получить код операции
      virtual SrValueCode code() const { return svvMemberVariable; }
    };







  //Константа целая
  struct SrValueConstInt : SrValue {
      int     mIValue;        //Целое значение или индекс строки

      SrValueConstInt( int val, SrType *type, const SrMark &mark ) : SrValue(mark), mIValue(val) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString::number(mIValue); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvConstInt; }

      void    setConst( SrType *type ) { mType = type; mConst = true; mConstInt = mIValue; }
    };




  //Постоянная строка
  struct SrValueConstString : SrValue {
      int mIndex; //Индекс строки в таблице строк

      SrValueConstString( int index, SrType *type, const SrMark &mark ) : SrValue(mark), mIndex(index) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString("cstring[%1]").arg(mIndex); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvConstString; }
    };





  //Унарная операция
  struct SrValueUnary : public SrValue {
      SrValue *mOperand;

      SrValueUnary( SrValue *oper, const SrMark &mark ) :
        SrValue( mark ), mOperand(oper) {}
      ~SrValueUnary() { if( mOperand ) delete mOperand; }

      //Получить строку листинга
      virtual QString     listing() override { if( mOperand ) return operation().arg( mOperand->listing() ); return operation().arg("ERR"); }

      //Получить значение операции
      virtual QString     operation() const = 0;

      //Результат константной операции
      virtual int         constOperation( int val ) { Q_UNUSED(val) return 0; }

      //Получить тип операции
      virtual SrType     *getType() override { if( mType == 0 && mOperand ) mType = mOperand->getType(); return mType; }

      //Вычислить константу
      void                checkConst() { mConst = mOperand && mOperand->mConst; if( mConst ) mConstInt = constOperation( mOperand->mConstInt ); }
    };




  //Разименовать указатель
  struct SrValuePointer : public SrValueUnary {
      SrValuePointer( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) { }

      //Получить значение операции
      virtual QString     operation() const override { return QString(" *(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPointer; }

      //Получить тип операции
      virtual SrType     *getType() override {
        if( mType == 0 && mOperand ) {
          mType = mOperand->getType();
          if( mType )
            mType = mType->mBaseType;
          }
        return mType;
        }

    };




  //Взять адрес
  struct SrValueAddress : public SrValueUnary {
      SrValueAddress( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString     operation() const override { return QString(" &(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAddress; }

      //Получить тип операции
      virtual SrType     *getType() override {
        if( mType == 0 && mOperand ) {
          mType = mOperand->getType();
          if( mType )
            mType = mType->getTypePointer();
          }
        return mType;
        }
    };



  //Прединкремент
  struct SrValuePredInc : public SrValueUnary {
      SrValuePredInc( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ++(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPredInc; }
    };


  //Преддекремент
  struct SrValuePredDec : public SrValueUnary {
      SrValuePredDec( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" --(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPredDec; }
    };


  //Постинкремент
  struct SrValuePostInc : public SrValueUnary {
      SrValuePostInc( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)++ "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPostInc; }
    };


  //Постдекремент
  struct SrValuePostDec : public SrValueUnary {
      SrValuePostDec( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)-- "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPostDec; }
    };


  //Битовое отрицание
  struct SrValueBitNot : public SrValueUnary {
      SrValueBitNot( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ~(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvNot; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return ~val; }
    };


  //Логическое отрицание
  struct SrValueLogNot : public SrValueUnary {
      SrValueLogNot( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" !(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvLogNot; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return !val; }
    };


  //Изменение знака
  struct SrValueNeg : public SrValueUnary {
      SrValueNeg( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" -(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvNeg; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return -val; }
    };







  //Бинарная операция
  struct SrValueBinary : public SrValue {
      SrValue *mOperand1;
      SrValue *mOperand2;

      SrValueBinary( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValue( mark ), mOperand1(oper1), mOperand2(oper2) {}
      ~SrValueBinary() { if( mOperand1 ) delete mOperand1; if( mOperand2 ) delete mOperand2; }


      //Получить строку листинга
      virtual QString listing() override { if( mOperand1 && mOperand2 ) return operation().arg( mOperand1->listing() ).arg( mOperand2->listing() ); return operation().arg("ERR").arg("ERR"); }

      //Получить значение операции
      virtual QString operation() const = 0;

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) { Q_UNUSED(val1) Q_UNUSED(val2) return 0; }

      //Вычислить константу
      void            checkConst() { mConst = mOperand1 && mOperand1->mConst && mOperand2 && mOperand2->mConst;
                                    if( mConst ) mConstInt = constOperation( mOperand1->mConstInt, mOperand2->mConstInt );
                                    }

      //Получить тип операции
      virtual SrType *getType() override { if( mType == 0 && mOperand1 ) mType = mOperand1->getType(); return mType; }
    };


  //Операция сохранения
  struct SrValueStore : public SrValueBinary {
      SrValueStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) = (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvStore; }
    };


  //Операция индекса массива
  struct SrValueArrayCell : public SrValueBinary {
      SrValueArrayCell( SrValue *array, const SrMark &mark, SrValue *index = 0 ) :
        SrValueBinary( array, mark, index ) { }

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)[(%2)] "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvArrayCell; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 + val2 * mType->mBaseType->mSize; }

      //Получить тип операции
      virtual SrType *getType() override {
        if( mType == 0 && mOperand1 ) {
          mType = mOperand1->getType();
          if( mType )
            mType = mType->mBaseType;
          }
        return mType;
        }
    };


  //Умножение
  struct SrValueMul : public SrValueBinary {
      SrValueMul( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) * (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvMul; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 * val2; }
    };


  //Умножение и сохранение
  struct SrValueMulStore : public SrValueBinary {
      SrValueMulStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) *= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvMulStore; }
    };


  //Деление
  struct SrValueDiv : public SrValueBinary {
      SrValueDiv( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) / (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvDiv; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val2 != 0 ? val1 / val2 : 0; }
    };


  //Деление и сохранение
  struct SrValueDivStore : public SrValueBinary {
      SrValueDivStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) /= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvDivStore; }
    };

  struct SrValueMod : public SrValueBinary {
      SrValueMod( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) mod (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvMod; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val2 != 0 ? val1 % val2 : 0; }
    };

  struct SrValueModStore : public SrValueBinary {
      SrValueModStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) mod= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvModStore; }
    };

  struct SrValueAdd : public SrValueBinary {
      SrValueAdd( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) + (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAdd; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 + val2; }
    };

  struct SrValueAddStore : public SrValueBinary {
      SrValueAddStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) += (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAddStore; }
    };

  struct SrValueSub : public SrValueBinary {
      SrValueSub( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) - (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvSub; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 - val2; }
    };

  struct SrValueSubStore : public SrValueBinary {
      SrValueSubStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) -= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvSubStore; }
    };

  struct SrValueLShift : public SrValueBinary {
      SrValueLShift( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) << (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvLShift; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 << val2; }
    };

  struct SrValueLShiftStore : public SrValueBinary {
      SrValueLShiftStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) <<= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvLShiftStore; }
    };

  struct SrValueRShift : public SrValueBinary {
      SrValueRShift( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) >> (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvRShift; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 >> val2; }
    };

  struct SrValueRShiftStore : public SrValueBinary {
      SrValueRShiftStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) >>= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvRShiftStore; }
    };

  struct SrValueOrStore : public SrValueBinary {
      SrValueOrStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) |= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvOrStore;  }
    };

  struct SrValueAndStore : public SrValueBinary {
      SrValueAndStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) &= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAndStore; }
    };

  struct SrValueXorStore : public SrValueBinary {
      SrValueXorStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) ^= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvXorStore; }
    };

  struct SrValueEqu : public SrValueBinary {
      SrValueEqu( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) == (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 == val2; }
    };

  struct SrValueNotEqu : public SrValueBinary {
      SrValueNotEqu( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) != (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvNotEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 != val2; }
    };

  struct SrValueLessEqu : public SrValueBinary {
      SrValueLessEqu( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) <= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvLessEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 <= val2; }
    };

  struct SrValueLess : public SrValueBinary {
      SrValueLess( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) < (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvLess; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 < val2; }
    };

  struct SrValueGreat : public SrValueBinary {
      SrValueGreat( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) > (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvGreat; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 > val2; }
    };

  struct SrValueGreatEqu : public SrValueBinary {
      SrValueGreatEqu( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) >= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvGreatEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 >= val2; }
    };



  //Бинарная операция для ленивых логических операций
  struct SrValueBinaryLong : public SrValue {
      SrValuePtr mOperand[SV_OPER_MAX];   //Операнды
      int        mOperCount;              //Количество операндов

      SrValueBinaryLong( SrValue *oper1, SrType *type, const SrMark &mark );

      ~SrValueBinaryLong();

      //Получить указатель на следующий операнд
      SrValuePtr& nextOperand();

      //Проверить доступность операндов
      bool        isOperandAvailable() const { return mOperCount < SV_OPER_MAX - 1; }

      //Получить строку листинга
      virtual QString listing();

      //Получить значение операции
      virtual QString operation() const = 0;

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) { Q_UNUSED(val1) Q_UNUSED(val2) return 0; }
    };


  //Логическое И
  struct SrValueLogAnd : public SrValueBinaryLong {
      int    mExitLabel;
      SrValueLogAnd( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" && "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvLogAnd; }
    };


  //Логическое ИЛИ
  struct SrValueLogOr : public SrValueBinaryLong {
      int    mExitLabel;
      SrValueLogOr( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" || "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvLogOr; }
    };


  //Бинарное ИЛИ
  struct SrValueOr : public SrValueBinaryLong {
      SrValueOr( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" | "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvOr; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) { return val1 | val2; }
    };


  //Бинарное И
  struct SrValueAnd : public SrValueBinaryLong {
      SrValueAnd( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" & "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvAnd; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) { return val1 & val2; }

    };


  //Бинарное Исключающее ИЛИ
  struct SrValueXor : public SrValueBinaryLong {
      SrValueXor( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" ^ "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvXor; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) { return val1 ^ val2; }
    };


  //Операция ,
  struct SrValueComma : public SrValueBinaryLong {
      SrValueComma( SrValue *oper1, const SrMark &mark ) :
        SrValueBinaryLong( oper1, 0, mark ) { mType = oper1->mType; }

      //Получить значение операции
      virtual QString operation() const { return QString(" , "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvComma; }

      //Получить тип операции
      virtual SrType *getType() override { if( mType == 0 && mOperand[mOperCount-1] ) mType = mOperand[mOperCount-1]->getType(); return mType; }
    };



  //Тройная условная операция
  struct SrValueCondition : public SrValue {
      SrValue *mCondition;
      SrValue *mTrue;
      SrValue *mFalse;
      int      mFalseAddress;
      int      mExitAddress;

      SrValueCondition( SrValue *cond, const SrMark &mark ) :
        SrValue(mark), mCondition(cond), mTrue(0), mFalse(0), mFalseAddress(0), mExitAddress(0) {}

      ~SrValueCondition() { if( mCondition ) delete mCondition; if( mTrue ) delete mTrue; if( mFalse ) delete mFalse; }

      //Получить строку листинга
      virtual QString listing() { if( mCondition && mTrue && mFalse ) return QString(" (%1 ? %2 : %3) ").arg( mCondition->listing() ).arg( mTrue->listing() ).arg( mFalse->listing() ); return QString("ERR"); }

      //Получить код операции
      virtual SrValueCode code() const { return svvCondition; }

      //Получить тип операции
      virtual SrType *getType() override { if( mType == 0 && mTrue ) mType = mTrue->getType(); return mType; }
    };




  //Вызов функции
  struct SrValueCall : public SrValue {
      SrValue    *mFunction;
      SrValuePtr  mParam[SV_FUN_PARAM_MAX];  //Параметры функции
      int         mParamCount;               //Количество параметров

      SrValueCall( SrValue *fun, SrType *result, const SrMark &mark );

      ~SrValueCall();

      SrValuePtr& nextParam();

      bool        isParamAvailable() const { return mParamCount < SV_FUN_PARAM_MAX - 1; }

      //Получить строку листинга
      virtual QString listing();

      //Получить код операции
      virtual SrValueCode code() const { return svvCall; }

      //Получить тип операции
      virtual SrType *getType() override;
    };













  //Оператор
  struct SrOperator {
      SrMark      mMark;   //Маркер начала оператора
      QString     mRemark; //Исходная строка в начале оператора
      SrOperator *mParent; //Родительский оператор

      SrOperator( const SrMark &src, const QString remark ) : mMark(src), mRemark(remark), mParent(0) {}
      virtual ~SrOperator() {}

      //Сформировать листинг
      virtual QString     listing( int level ) = 0;

      //Получить строку с заданным количеством пробелов
      QString             padding( int level ) const { return QString(level,QChar(' ')); }

      //Получить тип операции
      virtual SrStatement statement() const = 0;
    };




  //Оператор возврата из функции
  struct SrOperatorReturn : public SrOperator {
      SrValue    *mResult;   //Значение возвращаемого результата
      SrFunction *mFunction; //Функция, из которой осуществляется возврат

      SrOperatorReturn( const SrMark &src, const QString remark, SrFunction *fun );
      ~SrOperatorReturn();

      //Сформировать листинг
      virtual QString listing( int level ) override;

      //Получить тип операции
      virtual SrStatement statement() const override { return tstReturn; }

    };


  //Оператор установки маски исключения
  struct SvOperatorCatch : public SrOperator {
      int mMask;

      SvOperatorCatch( const SrMark &src, const QString remark ) : SrOperator( src, remark ) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("catch %1\n").arg(mMask,0,16); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstCatch; }

    };


  //Оператор генерации исключения
  struct SvOperatorThrow : public SrOperator {
      int mMask;

      SvOperatorThrow( const SrMark &src, const QString remark ) : SrOperator( src, remark ) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("throw %1\n").arg(mMask,0,16); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstThrow; }

    };




  struct SrOperatorContext : public SrOperator {
      SrOperatorContext *mParentContext; //Родительский контекст
      SrVariableList     mVarLocal; //Локальные переменные контекста

      SrOperatorContext( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperator(mark,remark), mParentContext(parent), mVarLocal(tsrLocal) {}

      SrVariable *getLocal( const QString &name ) {
        if( mVarLocal.isPresent(name) )
          return mVarLocal.getVariable(name);
        else if( mParentContext )
          return mParentContext->getLocal(name);
        return 0;
        }
    };




  typedef SrOperator *SrOperatorPtr;
  typedef QList<SrOperatorPtr> SrOperatorList;

  //Блоковый оператор - последовательность других операторов
  struct SrOperatorBlock : public SrOperatorContext {
      SrMark         mEnd;      //Место завершения блока в исходном файле
      SrOperatorList mList;     //Список операторов блока

      SrOperatorBlock( const SrMark &mark, const QString remark, SrOperatorContext *parent );

      ~SrOperatorBlock();

      //Сформировать листинг
      virtual QString listing( int level ) override;

      //Получить тип операции
      virtual SrStatement statement() const override { return tstBlock; }
    };

  //Оператор-выражение
  struct SrOperatorExpression : public SrOperator {
      SrValue *mExpression;

      SrOperatorExpression( const SrMark &mark, const QString remark ) : SrOperator( mark, remark ), mExpression(0) { }

      ~SrOperatorExpression() { if( mExpression ) delete mExpression; }

      //Сформировать листинг
      virtual QString listing( int level ) override { if( mExpression ) return padding(level) + mExpression->listing() + QString("\n"); return QString("ERR\n"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstExpression; }

    };

  //Условный оператор
  struct SrOperatorIf : public SrOperator {
      SrValue    *mCondition;
      SrOperator *mTrue;
      SrOperator *mFalse;
      int         mFalseAddress;
      int         mExitAddress;

      SrOperatorIf( const SrMark &mark, const QString remark ) :
        SrOperator(mark, remark), mCondition(0), mTrue(0), mFalse(0), mFalseAddress(0), mExitAddress(0) {}

      ~SrOperatorIf() { if(mCondition) delete mCondition; if(mTrue) delete mTrue; if(mFalse) delete mFalse; }

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("if "); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstIf; }

    };

  //Базовый оператор цикла
  struct SrOperatorLoop : public SrOperatorContext {
      SrValue    *mCondition;
      SrOperator *mBody;
      int         mExitAddress;
      int         mConditionAddress;

      SrOperatorLoop( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorContext( mark, remark, parent ), mCondition(0), mBody(0), mExitAddress(0), mConditionAddress(0) {}

      ~SrOperatorLoop() { if(mCondition) delete mCondition; if(mBody) delete mBody; }

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("loop"); }

    };

  //Оператор прекращения цикла
  struct SrOperatorBreak : public SrOperator {
      SrOperatorLoop *mLoop;

      SrOperatorBreak( const SrMark &mark, const QString remark, SrOperatorLoop *loop ) :
        SrOperator(mark, remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("break"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstBreak; }

    };

  //Оператор прекращения цикла
  struct SrOperatorContinue : public SrOperator {
      SrOperatorLoop *mLoop;

      SrOperatorContinue( const SrMark &mark, const QString remark, SrOperatorLoop *loop ):
        SrOperator(mark,remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("continue"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstContinue; }

    };


  //Оператор цикла while
  struct SrOperatorWhile : public SrOperatorLoop {

      SrOperatorWhile( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SrStatement statement() const override { return tstWhile; }

    };

  //Оператор цикла do while
  struct SrOperatorDoWhile : public SrOperatorLoop {

      SrOperatorDoWhile( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SrStatement statement() const override { return tstDo; }

    };

  //Оператор цикла for
  struct SrOperatorFor : public SrOperatorLoop {
      SrValue    *mInit;
      SrValue    *mAction;

      SrOperatorFor( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ), mInit(0), mAction(0) {}

      ~SrOperatorFor() { if(mInit) delete mInit; if(mAction) delete mAction; }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstFor; }

    };








  //Одна лексическая единица
  struct SrToken {
      int       mId       : 16; //Тип конструкции
      unsigned  mUnsigned : 1;  //Истина для установленного флага беззнакового числа
      unsigned  mLong     : 1;  //Истина для установленного флага длинного числа
      union {
          quint32 mUValue;
          qint32  mIValue;
          double  mDValue;
        };
      QString   mString;
      SrType   *mType;          //Указатель на тип, если детектирован тип

      int     mFileId;
      int     mLine;
      int     mColon;

      bool operator == ( int i ) const { return mId == i; }
      operator int () const { return mId; }
    };




  //Таблица источников данных
  typedef QStack<SrSourcePtr>           SvSourceStack;

  //Таблица байтовых массивов
  typedef QList<QByteArray>             SvByteArrayTable;









//Компилятор собственной персоной
class SrCompiler {
  public:
    //Раздел источников данных компиляции
    SvSourceStack            mSourceStack;    //Стек источников данных
    QMap<int,QString>        mFileTable;      //Список используемых файлов
    int                      mFileIdCount;    //Счетчик для генерации идентификаторов файлов
    int                      mFileId;         //ID обрабатываемого файла
    int                      mLineIndex;      //Индекс обрабатываемой строки

    //Раздел обработки исходного кода
    QString                  mLine;           //Обрабатываемая строка
    QList<int>               mLineInt;        //Соответствие позиций макрорасширенной строки позициям исходной строки
    int                      mPtr;            //Главный указатель кода (индекс в mLine)
    bool                     mEof;            //Главный флаг завершения кода
    QString                  mInLine;         //Место для хранения входной строки
    QString                  mMacroLine;      //Входная строка после макрорасширений
    QString                  mPrevRemark;     //Блок предварительных комментариев
    QString                  mAfterRemark;    //Блок последующих комментариев
    bool                     mCRemark;        //Флаг выдачи С кода в качестве коментария
    int                      mIfLevel;        //Текущий уровень условной компиляции
    int                      mSkipLevel;      //Уровень пропускаемой условной компиляции
    QString                  mName;           //Для сканирования имен
    SrMacroPtrTable          mMacroTable;     //Таблица макроимен

    int                      mVariant;        //Вариант контроллера
    int                      mVersion;        //Версия ПО контроллера
    int                      mGlobalAddress;  //Очередной свободный адрес глобальной переменной
    QStringList              mStringTable;    //Таблица строк
    SvByteArrayTable         mByteArrayTable; //Таблица байтовых массивов
    SrTypeList               mTypeList;       //Таблица типов
    SrVariableList           mVarGlobal;      //Список глобальных переменных
    SrFunctionList           mFunGlobal;      //Список глобальных функций

    int                      mLastStatement;  //Идентификатор последнего обработанного оператора
    SrFunction              *mActiveFunction; //Обрабатываемая функция
    SrStruct                *mActiveStruct;   //Обрабатываемая структура
    int                      mActiveImport;   //Обрабатывается import
    SrOperatorContext       *mContext;        //Текущий контекст
    SrOperatorLoop          *mLoop;           //Активный оператор цикла

    //Раздел таблицы ключевых слов
    QHash<QString,int>       mKeyWords;       //Список ключевых слов с токенами

    //Раздел типов
    SrType                  *mTypeInt;        //Целое
    SrType                  *mTypeIntPtr;     //Указатель на целое
    SrType                  *mTypeVoid;       //void
    SrType                  *mTypeVoidPtr;    //void ptr
    SrType                  *mTypeCString;    //Константная строка
    SrType                  *mTypeCStringPtr; //string ptr
    SrType                  *mTypeCBlock;     //Константный блок
    SrType                  *mTypeCBlockPtr;  //Указатель на константный блок
    SrType                  *mTypeFail;       //Недействительный тип

    //Раздел коментариев
    QList<QString>           mRemarkTable;    //Таблица коментариев

    SrToken                  mToken;          //Текущий токен

    //Распределение памяти
    int                      mLocalAddress;   //Очередной свободный адрес локальной переменной
    int                      mLocalMax;       //Максимальный адрес временных переменных внутри функции

    void                     InitKeyWords();
    void                     ClearTables();

  public:
    QString                  errorList();     //Преобразовать список ошибок к строке
    QStringList              mSystemPathList; //Путь к стандартным Include файлам
    QString                  mProjectPath;    //Путь к файлам проекта
  public:
            //Компилятор часть 1 (конструктор и инициализация)
    SrCompiler();
    virtual ~SrCompiler();

            void        Reset();

            //Компилятор часть 2 (входной поток низкого уровня)
            void         DoFile( const QString &fname, bool current );
    virtual SrSource*    CreateSource( const QString &fname, bool current );
            int          AddFile( const QString &fname );
            bool         IsEoln() const { return mPtr >= mLine.size(); }
            void         InLine();
            SrMark       mark() const;
            QString      curLine() const { return /*mLine.mid(mPtr); */ mLine; }

            //Компилятор часть 3 (поток ошибок)
            SvErrorList  mErrorList;
    virtual void         Error( const QString &err );
            void         ErrorInLine( const QString &err, int fileId, int lineId );
            void         errorInLine( const QString &err, const SrMark &mark );
            void         ErrorKillLine( const QString &err );
            void         ErrorEndSt( const QString &err );
            QString      Need( QChar ch ) { return QObject::tr("Error. Need '%1'.").arg(ch); }
            QString      Need( const QString &str ) { return QObject::tr("Error. Need %1.").arg(str); }
            void         ErrNeedLValue();    //Выдает ошибку с необходимостью LValue - значения
            void         ErrIllegalOperation(); //Выдает ошибку с недопустимостью операции с данными типами
            bool         needToken( int token, const QString need );

            //Компилятор часть 4 (препроцессор)
            void         Blank();
            void         White();
            bool         Match( const char *ptr ); //Если совпадает, то съедает
            bool         Match( char ch );    //Если совпадает, то съедает
            bool         ScanName();
            bool         ScanNameImp( int &ptr );
            bool         ScanInt();

    virtual void         Compile( const QString &fname ); //Точка входа в компиляцию
            void         DoPreprocess();
            void         DoInclude();
            void         DoExpand();
            void         DoDefine();
            void         IfLine();

            //Компилятор часть 4А (условие в препроцессоре #if)
            int          BConstCompare(); //Выполнить разбор константного сравнения и выдать его результат
            int          BccB1();
            int          BccB2();
            int          BccB3();
            int          BccB4();
            int          BccB5();
            int          BccB6();
            int          BccB7();
            int          BccB8();
            int          BccB9();
            int          BccB10();
            int          BccB11();

            //Компилятор часть 5 (сканер)
            bool         ScanDigit();     //Проверить наличие числа и сканировать если есть
            bool         NextToken();     //Получить следующий токен
            void         NeedSemicolon(); //Проверить следующий токен на точку с запятой


            //Компилятор часть 6 (таблицы)
            //Коментарий
            void         AddAsmRemark(const QString &sour);          //Добавить коментарий, ассемблерный код
            //Таблица символов
            bool         isGlobalSymbol( const QString &name );

            //Компилятор часть 7 (разбор выражений)
            SrType*      ConstExpression(int *intRes);
            void         BExpression( SrValuePtr &val );
            void         B0( SrValuePtr &res );
            void         B1( SrValuePtr &val ); // Операция Условие
            void         B2( SrValuePtr &val ); // ||
            void         B3( SrValuePtr &val ); // &&
            void         B4( SrValuePtr &val ); // |
            void         B5( SrValuePtr &val ); // ^
            void         B6( SrValuePtr &val ); // &
            void         B7( SrValuePtr &val ); // == и !=
            void         B8( SrValuePtr &val ); // <=  >=  <  >
            void         B9( SrValuePtr &val ); // << и >>
            void         B10( SrValuePtr &val ); // + и -
            void         B11( SrValuePtr &val ); // *  /  %
            void         B12( SrValuePtr &val ); // ++val --val ~val !val -val +val *val &val val++ val--
            void         B13( SrValuePtr &val ); // [val] val()
            void         B14( SrValuePtr &val ); // val. val->
            void         B14Member( SrValuePtr &val );
            void         B15(SrValuePtr &val ); // (val) val()
            bool         BoWaitFunction(SrValuePtr &val );   //Реализация специальной функции Wait


            //Компилятор часть 8 (синтаксический разбор)
            void         DoCBlock();                        //Блок данных
            void         DoImport();                        //Объявление импортируемого объекта
            void         DoDeclareGlobal();                 //Объявление или определение переменных и функций
            void         DoStructure();                     //Определение структуры
            void         DoNewFunction(SrFunction *fun );   //Объявление/определение новой функции
            void         DoParamList(SrFunction *fun );     //Декодирование списка параметров
            void         DoParamAddress( SrFunction *fun ); //Назначить параметрам адреса
            //Определение подтипа
            void         DoSubType( SrType *type, SrVariablePtr &var, SrFunctionPtr &fun, int *addressCount );
            bool         DoMemberSubType( SrVariable& );     //Определение подтипа для членов структур
            SrType*      DoBaseType();                      //Определение основного типа
            SrOperator*  DoStatement(SrOperator *parent);   //Оператор
            void         ClearTemp( int savOffset );        //Очистить стек временных переменных
            SrOperator*  DoLocal();                         //Оператор определения локальных переменных
            SrOperatorBlock *DoCompound();                      //Оператор блок
            SrOperator*  DoIf();                            //Оператор if
            SrOperator*  DoWhile();                         //Оператор while
            SrOperator*  DoDo();                            //Оператор do
            SrOperator*  DoFor();                           //Оператор for
            SrOperator*  DoSwitch();                        //Оператор switch
            SrOperator*  DoCase();                          //Оператор case
            SrOperator*  DoDefault();                       //Оператор default
            SrOperator*  DoReturn();                        //Оператор return
            SrOperator*  DoBreak();                         //Оператор break
            SrOperator*  DoContinue();                      //Оператор continue
            SrOperator*  DoExpression();                    //Оператор выражение
            //void      DoTry();                           //Оператор блок с поиском исключений
            SrOperator*  DoCatch();                         //Оператор блок обработки исключений
            SrOperator*  DoThrow();                         //Оператор возбуждения исключений

            //Компилятор часть 9 вычисление константных выражений
            void         constValueCalc( SrValue *val );
            void         constValueLongCalc( SrValueBinaryLong *val );


            //Компилятор часть S (сцены)
            void         DoScene();                         //Сцена
            void         DoSceneBody( SvSmlScene *scene );  //Разобрать тело сцены
            void         ClearSceneList();                  //Очистить список сцен и создать по умолчанию
            void         DoSceneProperty();
            void         DoSceneSettings();
            void         DoSceneVint();
            void         DoSceneSignal();
            void         DoSceneFunction();
            void         DoSceneName();
            void         DoSetProperty( SvSmlProperty *prop );
            void         DoSetAlias( SvSmlProperty *prop );
            void         DoSceneParsing();
            void         DoPropertyAllocation();


    virtual int          AllocLocal( SrType *type );
            int          AllocGlobal( SrType *type );

            QString      Listing();

  };

}

#endif //SRCOMPILER_H

