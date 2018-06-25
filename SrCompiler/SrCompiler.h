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


  /*!
    \enum tXXX Коды Token сканера
    \brief Сканер простматривает входную программу и выдает ее как последовательность
    токенов. Синтаксическому анализатору требуется разбирать программу представленную в токенах.

    Для всех ключевых слов C++ предусмотрены токены, а также для трех дополнительных ключевых
    слов характерных для скрипта. Они помечены знаками !!!. Токены представлены также для
    знаков и конструкций из нескольких знаков.
   */
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


  /*!
    \enum classOfType классы типов данных
    \brief каждый из типов данных принадлежит к одному из приведенных классов
   */
  enum {
    CLASS_VOID         = 0x000001,
    CLASS_INT          = 0x000002,   //! Целое 32 бит
    CLASS_FUNCTION     = 0x000008,   //! Функция
    CLASS_STRUCT       = 0x000010,   //! Структура
    CLASS_CSTRING      = 0x000020,   //! Константная строка
    CLASS_CBLOCK       = 0x000040,   //! Блок константных данных (картинка, звук и т.п.)
    CLASS_POINTER      = 0x000080,   //! Указатель на что-нибудь
    CLASS_ARRAY        = 0x000100,   //! Массив элементов
    CLASS_LAST };






  /*!
     \brief The SrSort enum Вид размещения переменной.
     Переменная может быть размещена в глобальном пространстве,
     в локальном пространстве (в стеке) и быть членом какой-либо структуры.
   */
  enum SrSort {
    tsrGlobal, //! Переменная размещена в глобальном пространстве
    tsrLocal,  //! Переменная размещена в локальном пространстве
    tsrMember  //! Переменная является членом структуры
    };







  //Ссылка на элемент помощи
  /*!
     \brief The SrHelp struct Помощь по объекту.

     При своей работе компилятор собирает по исходному коду коментарии
     расположенные в непосредственной близости от определения функций,
     переменных и макросов. Эти коментарии составляют быструю помощь по
     элементу и используются системой проектирования IDE для написания
     и отладки программ rc++.
   */
  struct SrHelp {
      QString         mBrief;         //! Короткое описание
      QString         mReference;     //! Ссылка на документацию
      SrMark          mDefinePos;     //! Позиция определения

      //Конструктор пустой помощи
      SrHelp();

      SrHelp( const SrHelp &hlp );

      void update( const SrHelp &hlp );

      void clear();
    };



  /*!
     \brief The SrMacro struct Макрорасширение для макроопределений вместе
     с дополнительной информацией

     Данная структура хранит макрорасширение для макроса, а также дополнительную информацию.
     Метка помощи предназначена для показа IDE. Список использованных мест
     также предназначен для IDE. На основе этого списка может быть осуществлена автозамена
     объекта, а также локализованы места его использования
   */
  struct SrMacro {
      SrHelp     mHelp;          //! Помощь по макросу
      QString    mExpander;      //! Строка с макрорасширением
      SrMarkList mReferneceList; //! Список мест, в которых используется макрос
    };

  typedef SrMacro *SrMacroPtr;
  typedef QHash<QString,SrMacroPtr> SrMacroPtrTable;


  struct SrValue;
  typedef SrValue *SrValuePtr;
  typedef QList<SrValuePtr> SrValuePtrList;




  /*!
     \brief The SrValueList struct Список значений SrValue, которые представляют
     собой ветки дерева разбора выражений.
   */
  struct SrValueList {
      SrValuePtrList mList;

      SrValueList() : mList() {}

      ~SrValueList() { qDeleteAll(mList); }

      int     addExpression( SrValue *val ) { mList.append( val ); return mList.count() - 1; }

      void    clear() { qDeleteAll(mList); mList.clear(); }

      QString listing();
    };






  struct SrType;
  typedef SrType *SrTypePtr;


  /*!
     \brief The SrVariable struct Переменная в скрипте
   */
  struct SrVariable {
      SrHelp           mHelp;               //! Помощь по переменной
      QString          mName;               //! Имя переменной
      SrType          *mType;               //! Тип переменной
      SrSort           mSort;               //! Сорт (область) переменной: глобальная, локальная, член
      int              mAddress;            //! Адрес начала относительный
      int              mArraySize;          //! Размер массива (для массивов)
      SrMark           mMarkDefine;         //! Место определения
      QString          mRemark;             //! Описание переменной
      SrValue         *mInit;               //! Инициализационное значение
      SrMarkList       mReferenceList;      //! Список используемых переменной мест


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
  /*!
     \brief The SrVariableList struct Список переменных. Содержит собственно список переменных,
     хэш-таблицу для быстрого поиска переменных и сорт размещения переменных
   */
  struct SrVariableList {
      SrVariablePtrList mList; //! Последовательный список переменных
      SrVariablePtrHash mHash; //! Хэш-таблица быстрого поиска переменных
      SrSort            mSort; //! Вид таблицы переменных

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






  struct SrOperatorBlock;

  /*!
     \brief The SrFunction struct Описывает функцию в глобальном контексте
   */
  struct SrFunction {
      SrHelp           mHelp;               //! Помощь по функции
      QString          mName;               //! Имя функции
      bool             mDefined;            //! Флаг, выставляется, когда тело функции уже определено
      bool             mDeclared;           //! Флаг, выставляется, когда функция объявлена (например в классе)
                                            //! в глобальном контексте все функции объявлены по умолчанию
      int              mAddress;            //! Адрес начала кода
      int              mImportIndex;        //! Индекс импортной функции
      int              mParamSize;          //! Размер области параметров
      SrMark           mMarkDefine;         //! Место определения
      SrMark           mMarkDeclare;        //! Место первого объявления
      QString          mRemark;             //! Описание переменной
      SrVariableList   mParams;             //! Список параметров функции
      SrType          *mResultType;         //! Тип результата
      SrType          *mType;               //! Тип данной функции
      SrOperatorBlock *mBody;               //! Тело функции
      int              mLocalAmount;        //! Место под локальные переменные
      SrMarkList       mReferenceList;      //! Список мест использования функции

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





  /*!
     \brief The SrFunctionList struct Список функций. Содержи собственно список функций,
     а также хэш-таблицу для быстрого доступа к функции
   */
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

  /*!
     \brief The SrType struct Основной и производный тип

     Все объекты скрипта имеют какой-либо тип. При инициализации
     компилятор создает и регистрирует набор базовых типов. Из этих базовых
     типов может создаваться новый тип.

     Имя типа - это то имя, которым тип представлен в программе.

     Сигнатура типа формируется из сигнатуры базового типа путем добавления модификаторов.
     Сигнатура нужна для определения одинаковых типов, но имеющих разные имена. Это актуально
     для typedef-ов (в настоящее время не используются) и для функций. Функции одинаковые по
     параметрам и по результату будут иметь одинаковую сигнатуру.
   */
  struct SrType {
      QString            mName;           //! Первое имя типа
      QString            mSignature;      //! Сигнатура типа
      int                mClass;          //! Основной тип (класс)
      int                mNumElem;        //! Количество элементов массива
      int                mSize;           //! Размер объекта данного типа
      SrType            *mBaseType;       //! Базовый тип для производных типов
      SrHelp             mHelp;           //! Помощь по типу
      SrTypeList        *mTypeList;       //! Список всех типов для генерации новых типов из данного
                                          //! Этот список содержится в компиляторе и предоставляется
                                          //! единый на весь компилятор.

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

      bool            isInt()     const { return mClass == CLASS_INT; }
      bool            isVoid()    const { return mClass == CLASS_VOID; }
      bool            isArray()   const { return mClass == CLASS_ARRAY; }
      bool            isPointer() const { return mClass == CLASS_POINTER; }
      bool            isCBlock()  const { return mClass == CLASS_CBLOCK; }
      bool            isCString() const { return mClass == CLASS_CSTRING; }
      bool            isStruct()  const { return mClass == CLASS_STRUCT; }

      //Проверить соответствие параметра
      bool            isMatchParam(SrType *src , bool srcNull);

      //Построить сигнатуру
      virtual QString buildSignature();

      //Построить листинг
      virtual QString listing();

    };






  typedef QList<SrTypePtr> SvTypePtrList;
  typedef QHash<QString,SrTypePtr> SvTypePtrHash;



  /*!
     \brief The SrTypeList class Список типов. Содержит список всех типов, а также
     две хэш-таблицы для быстрого доступа к типам. Одна из таблиц производит
     выборку по именам типов, а другая - по сигнатурам.

   */
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








  /*!
     \brief The SrFunParam struct Формальный параметр функции в описании типа функции
   */
  struct SrFunParam {
      QString          mName;               //! Формальное имя параметра
      SrType          *mType;               //! Тип параметра

      SrFunParam() : mName(), mType(0) {}

      SrFunParam( SrVariable *var ) : mName(var->mName), mType(var->mType) {}

      SrFunParam& operator = ( const SrFunParam &p ) { mName = p.mName; mType = p.mType; return *this; }
    };

  typedef QList<SrFunParam> SrParamList;




  /*!
     \brief The SrFunctionType struct Тип описывающий функцию. В дополнение к базовому типу
     в типе-функции присутствует список формальных параметров и результат.
   */
  struct SrFunctionType : public SrType {
      SrParamList      mParamList;      //Список параметров
      SrType          *mResult;         //Тип результата
      int              mParamSize;      //Размер области параметров

      SrFunctionType() : mParamList(), mResult(0), mParamSize(0) { mClass = CLASS_FUNCTION; }
      ~SrFunctionType();

      //Построить сигнатуру
      virtual QString buildSignature() override;
    };






  /*!
     \brief The SrStruct struct Тип описывающий структуру. В дополнение к базовому типу
     в типе-структуре присутствует список членов и указатель на наследованную структуру.
   */
  struct SrStruct : public SrType {
      bool               mDefined;        //! Структура определена
      SrStruct          *mBaseStruct;     //! Базовая структура, от которой наследовалась данная
      SrVariableList     mMemberList;     //! Список всех членов. При создании структуры в этот
                                          //! спислк копируется список членов из наследовуемой
                                          //! структуры

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









  /*!
     \brief The SrSource class Источник исходного кода.

     При компиляции из отдельной программы скрипт, как правило, читается
     из файла. Поэтому по умолчанию при обнаружении ссылки на какой-либо
     файл компилятор открывает его с диска. Однако, при использовании
     IDE может быть полезным получать содержимое файла не с диска, а
     прямо из редактора. В этом случае соответствующие функции компилятора должны
     быть переопределены.
   */
  class SrSource {
    protected:
      int          mLineCount;   //! Счетчик строк исходного файла
      int          mFileId;      //! Идентификатор файла в общей таблице файлов
      QTextStream *mInputStream; //! Исходный код в виде потока
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



  /*!
     \brief The SrSourceFile class Источник исходного кода в виде файла на диске.
   */
  class SrSourceFile : public SrSource {
      QFile       *mFile;
    public:
      SrSourceFile( QFile *file, int fileId );
      ~SrSourceFile();
    };





  /*!
     \brief The SrStatement enum Коды идентификации операторов

     Компилятор может выполнять некоторую оптимизацию в зависимости от
     порядка следования операторов.
   */
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
    tstBlock,
    tstDefault };


  /*!
     \brief The SrValueCode enum Коды идентификации операций в дереве разбора выражений
   */
  enum SrValueCode {
    svvVariable,        //Переменная (глобальная, локальная, параметр)
    svvFunction,        //Функция (глобальная)
    svvWaitFun,         //Специальная функция srWait
    svvThrowFun,        //Специальная функция srThrow
    svvCatchFun,        //Специальная функция srCatch
    svvExceptionFun,    //Специальная функция srException
    svvMemberVariable,  //Член от произвольной структуры
    svvConstInt,        //Константа int
    svvConstString,     //! Константная строка
    svvPointer,         //! Берет значение по указателю
    svvAddress,         //! Адрес какого-либо объекта
    svvPredInc,
    svvPredDec,
    svvPostInc,
    svvPostDec,
    svvNot,
    svvLogNot,
    svvNeg,
    svvStore,
    svvArrayCell,         //Операция индекса массива
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




  /*!
     \brief The SrValue struct Ветка и лист в дереве разбора выражений

     Все выражения представлены экземпляром структуры SrValue. В зависимости
     от фактической наследованной структуры SrValueXXX выражение представляется
     в виде дерева.
   */
  struct SrValue {
      SrType      *mType;        //Тип значения, полученного в результате
      SrMark       mMark;        //Место в файле для данного значения
      int          mConstInt;    //Вычисленный результат
      bool         mConst;       //Флаг показывает константный результат

      SrValue( SrType *type, const SrMark &mark );
      virtual ~SrValue() {}

      void    clear() { memset( this, 0, sizeof(SrValue) ); }

      //Проверка константного значения
      bool    isConst() const { return mConst; }

      //Преобразовать к значению условия
      bool    toCondition() const { return mConstInt; }

      //Получить целую константу
      int     toConstInt() const { return mConstInt; }


      //Получить строку листинга
      virtual QString     listing() = 0;

      //Получить код операции
      virtual SrValueCode code() const = 0;

      //Получить тип операции
      virtual SrType*     getType() { return mType; }

      //Получить класс типа результата
              int         getClass() { return getType() == 0 ? 0 : mType->mClass; }
  };




  /*!
     \brief The SrValueVariable struct Переменная (глобальная, локальная, параметр)
   */
  struct SrValueVariable : SrValue {
      SrVariable *mVariable;
      int         mAddonAddress;

      SrValueVariable( SrVariable *var, const SrMark &mark );

      //Получить строку листинга
      virtual QString     listing() { if( mVariable ) return QString(" <%1> ").arg(mVariable->printListing()); return QString("<ERR>"); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvVariable; }

      //Получить тип операции
      virtual SrType     *getType() override;
    };




  /*!
     \brief The SrValueFunction struct Взятие адреса функции (глобальной)
   */
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





  /*!
     \brief The SrValueWaitFun struct Специальная функция srWait
   */
  struct SrValueWaitFun : SrValue {
      SrValueWaitFun( SrType *type, const SrMark &mark ) : SrValue(mark) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" srWait() "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvWaitFun; }
    };


  /*!
     \brief The SrValueThrowFun struct Специальная функция srThrow
   */
  struct SrValueThrowFun : SrValue {
      SrValue *mThrowCode;

      SrValueThrowFun( SrValue *throwCode, SrType *type, const SrMark &mark ) : SrValue(mark), mThrowCode(throwCode) { mType = type; }
      ~SrValueThrowFun() { delete mThrowCode; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" srThrow(%1) ").arg( mThrowCode->listing() ); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvThrowFun; }
    };


  /*!
     \brief The SrValueCatchFun struct Специальная функция srCatch
   */
  struct SrValueCatchFun : SrValue {
      SrValue *mCatchMask;

      SrValueCatchFun( SrValue *catchMask, SrType *type, const SrMark &mark ) : SrValue(mark), mCatchMask(catchMask) { mType = type; }
      ~SrValueCatchFun() { delete mCatchMask; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" srCatch(%1) ").arg( mCatchMask->listing() ); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvCatchFun; }
    };



  /*!
     \brief The SrValueExceptionFun struct Специальная функция srException
   */
  struct SrValueExceptionFun : SrValue {
      SrValueExceptionFun( SrType *type, const SrMark &mark ) : SrValue(mark) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" srException() "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvExceptionFun; }
    };




  /*!
     \brief The SrValueMemberVariable struct Член от произвольной структуры
   */
  struct SrValueMemberVariable : SrValueVariable {
      SrValue *mStruct; //Выражение вычисления структуры

      SrValueMemberVariable( SrVariable *var, SrValue *struc, const SrMark &mark );
      ~SrValueMemberVariable();

      //Получить строку листинга
      virtual QString     listing() override { if(mStruct) return mStruct->listing() + QString(".") + SrValueVariable::listing(); return QString("ERR.") + SrValueVariable::listing(); }

      //Получить код операции
      virtual SrValueCode code() const { return svvMemberVariable; }
    };







  /*!
     \brief The SrValueConstInt struct Константа целая
   */
  struct SrValueConstInt : SrValue {
      int     mIValue;        //Целое значение или индекс строки

      SrValueConstInt( int val, SrType *type, const SrMark &mark ) : SrValue(mark), mIValue(val) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString::number(mIValue); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvConstInt; }

      void    setConst( SrType *type ) { mType = type; mConst = true; mConstInt = mIValue; }
    };




  /*!
     \brief The SrValueConstString struct Постоянная строка
   */
  struct SrValueConstString : SrValue {
      int mIndex; //Индекс строки в таблице строк

      SrValueConstString( int index, SrType *type, const SrMark &mark ) : SrValue(mark), mIndex(index) { mType = type; }

      //Получить строку листинга
      virtual QString     listing() override { return QString("cstring[%1]").arg(mIndex); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvConstString; }
    };





  /*!
     \brief The SrValueUnary struct Унарная операция
   */
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




  /*!
     \brief The SrValuePointer struct Разименовать указатель
   */
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




  /*!
     \brief The SrValueAddress struct Взять адрес
   */
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





  /*!
     \brief The SrValuePredInc struct Прединкремент
   */
  struct SrValuePredInc : public SrValueUnary {
      SrValuePredInc( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ++(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPredInc; }
    };





  /*!
     \brief The SrValuePredDec struct Преддекремент
   */
  struct SrValuePredDec : public SrValueUnary {
      SrValuePredDec( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" --(%1) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPredDec; }
    };





  /*!
     \brief The SrValuePostInc struct Постинкремент
   */
  struct SrValuePostInc : public SrValueUnary {
      SrValuePostInc( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)++ "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPostInc; }
    };





  /*!
     \brief The SrValuePostDec struct Постдекремент
   */
  struct SrValuePostDec : public SrValueUnary {
      SrValuePostDec( SrValue *oper, const SrMark &mark ) :
        SrValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)-- "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvPostDec; }
    };





  /*!
     \brief The SrValueBitNot struct Битовое отрицание
   */
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





  /*!
     \brief The SrValueLogNot struct Логическое отрицание
   */
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





  /*!
     \brief The SrValueNeg struct Изменение знака
   */
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







  /*!
     \brief The SrValueBinary struct Бинарная операция
   */
  struct SrValueBinary : public SrValue {
      SrValue *mOperand1;
      SrValue *mOperand2;

      SrValueBinary( SrValue *oper1, const SrMark &mark, SrValue *oper2 = nullptr ) :
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





  /*!
     \brief The SrValueStore struct Операция сохранения
   */
  struct SrValueStore : public SrValueBinary {
      SrValueStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) = (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvStore; }
    };





  /*!
     \brief The SrValueArrayCell struct Операция индекса массива
   */
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





  /*!
     \brief The SrValueMul struct Умножение
   */
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





  /*!
     \brief The SrValueMulStore struct Умножение и сохранение
   */
  struct SrValueMulStore : public SrValueBinary {
      SrValueMulStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) *= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvMulStore; }
    };





  /*!
     \brief The SrValueDiv struct Деление
   */
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





  /*!
     \brief The SrValueDivStore struct Деление и сохранение
   */
  struct SrValueDivStore : public SrValueBinary {
      SrValueDivStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) /= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvDivStore; }
    };






  /*!
     \brief The SrValueMod struct Остаток от деления
   */
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






  /*!
     \brief The SrValueModStore struct Остаток от деления и сохранение
   */
  struct SrValueModStore : public SrValueBinary {
      SrValueModStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) mod= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvModStore; }
    };






  /*!
     \brief The SrValueAdd struct Сложение
   */
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






  /*!
     \brief The SrValueAddStore struct Сложение и сохранение
   */
  struct SrValueAddStore : public SrValueBinary {
      SrValueAddStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) += (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAddStore; }
    };





  /*!
     \brief The SrValueSub struct Вычитание
   */
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






  /*!
     \brief The SrValueSubStore struct Вычитание и сохранение
   */
  struct SrValueSubStore : public SrValueBinary {
      SrValueSubStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) -= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvSubStore; }
    };





  /*!
     \brief The SrValueLShift struct Сдвиг влево
   */
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






  /*!
     \brief The SrValueLShiftStore struct Сдвиг влево и сохранение
   */
  struct SrValueLShiftStore : public SrValueBinary {
      SrValueLShiftStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) <<= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvLShiftStore; }
    };





  /*!
     \brief The SrValueRShift struct Сдвиг вправо
   */
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





  /*!
     \brief The SrValueRShiftStore struct Сдвиг вправо и сохранение
   */
  struct SrValueRShiftStore : public SrValueBinary {
      SrValueRShiftStore( SrValue *oper1, const SrMark &mark, SrValue *oper2 = 0 ) :
        SrValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) >>= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvRShiftStore; }
    };





  /*!
     \brief The SrValueOrStore struct Бинарное ИЛИ и сохранение
   */
  struct SrValueOrStore : public SrValueBinary {
      SrValueOrStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) |= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvOrStore;  }
    };




  /*!
     \brief The SrValueAndStore struct Бинарное И и сохранение
   */
  struct SrValueAndStore : public SrValueBinary {
      SrValueAndStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) &= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvAndStore; }
    };





  /*!
     \brief The SrValueXorStore struct Бинарное ХОР и сохранение
   */
  struct SrValueXorStore : public SrValueBinary {
      SrValueXorStore( SrValue *oper1, const SrMark &mark ) :
        SrValueBinary( oper1, mark, 0 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) ^= (%2) "); }

      //Получить код операции
      virtual SrValueCode code() const override { return svvXorStore; }
    };





  /*!
     \brief The SrValueEqu struct Равенство
   */
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





  /*!
     \brief The SrValueNotEqu struct Неравенство
   */
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





  /*!
     \brief The SrValueLessEqu struct Меньше или равно
   */
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





  /*!
     \brief The SrValueLess struct Меньше
   */
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





  /*!
     \brief The SrValueGreat struct Больше
   */
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





  /*!
     \brief The SrValueGreatEqu struct Больше или равно
   */
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






  /*!
     \brief The SrValueBinaryLong struct Бинарная операция для ленивых логических операций
   */
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






  /*!
     \brief The SrValueLogAnd struct Логическое И
   */
  struct SrValueLogAnd : public SrValueBinaryLong {
      int    mExitLabel;
      SrValueLogAnd( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" && "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvLogAnd; }
    };






  /*!
     \brief The SrValueLogOr struct Логическое ИЛИ
   */
  struct SrValueLogOr : public SrValueBinaryLong {
      int    mExitLabel;
      SrValueLogOr( SrValue *oper1, SrType *type, const SrMark &mark ) :
        SrValueBinaryLong( oper1, type, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const { return QString(" || "); }

      //Получить код операции
      virtual SrValueCode code() const { return svvLogOr; }
    };






  /*!
     \brief The SrValueOr struct Бинарное ИЛИ
   */
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






  /*!
     \brief The SrValueAnd struct Бинарное И
   */
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






  /*!
     \brief The SrValueXor struct Бинарное Исключающее ИЛИ
   */
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






  /*!
     \brief The SrValueComma struct Операция ,
   */
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







  /*!
     \brief The SrValueCondition struct Тройная условная операция
   */
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





  /*!
     \brief The SrValueCall struct Вызов функции
   */
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














  /*!
     \brief The SrOperator struct Базовая конструкция оператора. Все
     операторы выполняются последовательно, т.е. имеют один вход и один выход.
     Внутри некоторых операторов может быть блок других операторов.
   */
  struct SrOperator {
      SrMark      mMark;   //! Маркер начала оператора
      QString     mRemark; //! Исходная строка в начале оператора
      SrOperator *mParent; //! Родительский оператор

      SrOperator( const SrMark &src, const QString remark ) : mMark(src), mRemark(remark), mParent(0) {}
      virtual ~SrOperator() {}

      //Сформировать листинг
      virtual QString     listing( int level ) = 0;

      //Получить строку с заданным количеством пробелов
      QString             padding( int level ) const { return QString(level,QChar(' ')); }

      //Получить тип операции
      virtual SrStatement statement() const = 0;
    };






  /*!
     \brief The SrOperatorReturn struct Оператор возврата из функции
   */
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






  /*!
     \brief The SrOperatorContext struct Базовый оператор для операторов,
     имеющих внутри себя собственный котекст. Примером такого оператора является
     for, который позволяет определять переменные прямо внутри условия.
   */
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

  /*!
     \brief The SrOperatorBlock struct Блоковый оператор - последовательность других операторов
   */
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






  /*!
     \brief The SrOperatorExpression struct Оператор-выражение
   */
  struct SrOperatorExpression : public SrOperator {
      SrValue *mExpression;

      SrOperatorExpression( const SrMark &mark, const QString remark ) : SrOperator( mark, remark ), mExpression(0) { }

      ~SrOperatorExpression() { if( mExpression ) delete mExpression; }

      //Сформировать листинг
      virtual QString listing( int level ) override { if( mExpression ) return padding(level) + mExpression->listing() + QString("\n"); return QString("ERR\n"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstExpression; }

    };





  /*!
     \brief The SrOperatorIf struct Условный оператор
   */
  struct SrOperatorIf : public SrOperator {
      SrValue    *mCondition;     //! Выражение вычисления условия
      SrOperator *mTrue;          //! Оператор, который выполняется если условие истино
      SrOperator *mFalse;         //! Оператор, который выполняется если условие ложно
      int         mFalseAddress;  //! Метка для перехода на оператор mFalse
      int         mExitAddress;   //! Метка для выхода из условия (обход ветки mFalse)

      SrOperatorIf( const SrMark &mark, const QString remark ) :
        SrOperator(mark, remark), mCondition(0), mTrue(0), mFalse(0), mFalseAddress(0), mExitAddress(0) {}

      ~SrOperatorIf() { if(mCondition) delete mCondition; if(mTrue) delete mTrue; if(mFalse) delete mFalse; }

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("if "); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstIf; }

    };





  /*!
     \brief The SrOperatorLoop struct Базовый оператор цикла
   */
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




  /*!
     \brief The SrOperatorBreak struct Оператор прекращения цикла
   */
  struct SrOperatorBreak : public SrOperator {
      SrOperatorLoop *mLoop;

      SrOperatorBreak( const SrMark &mark, const QString remark, SrOperatorLoop *loop ) :
        SrOperator(mark, remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("break"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstBreak; }

    };




  /*!
     \brief The SrOperatorContinue struct Оператор перехода к условию цикла
   */
  struct SrOperatorContinue : public SrOperator {
      SrOperatorLoop *mLoop;

      SrOperatorContinue( const SrMark &mark, const QString remark, SrOperatorLoop *loop ):
        SrOperator(mark,remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("continue"); }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstContinue; }

    };




  /*!
     \brief The SrOperatorWhile struct Оператор цикла while
   */
  struct SrOperatorWhile : public SrOperatorLoop {

      SrOperatorWhile( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SrStatement statement() const override { return tstWhile; }

    };




  /*!
     \brief The SrOperatorDoWhile struct Оператор цикла do while
   */
  struct SrOperatorDoWhile : public SrOperatorLoop {

      SrOperatorDoWhile( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SrStatement statement() const override { return tstDo; }

    };




  /*!
     \brief The SrOperatorFor struct Оператор цикла for
   */
  struct SrOperatorFor : public SrOperatorLoop {
      SrValue    *mInit;
      SrValue    *mAction;

      SrOperatorFor( const SrMark &mark, const QString remark, SrOperatorContext *parent ) :
        SrOperatorLoop( mark, remark, parent ), mInit(0), mAction(0) {}

      ~SrOperatorFor() { if(mInit) delete mInit; if(mAction) delete mAction; }

      //Получить тип операции
      virtual SrStatement statement() const override { return tstFor; }

    };








  /*!
     \brief The SrToken struct Одна лексическая единица (выход работы сканера)
   */
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









  /*!
     \brief The SrCompiler class Компилятор собственной персоной

     Выполняет разбор исходного текста скрипта и формирует различные таблицы, на основе которых
     осуществляется генерация байт-кода.

     Компилятор выполнен по технологии рекурсивного спуска. Изначальными корнями уходит
     в SmallC Compiler лохматого года.

     В качестве промежуточного кода с версии 6 используется дерево. Ранее были триады.
     Дерево дает больше информации по откомпилированной программе. Оно позволяет
     проще проводить оптимизацию, а также упростило синтактический разбор.
   */
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


    public:
      QString                  errorList();     //Преобразовать список ошибок к строке
      QStringList              mSystemPathList; //Путь к стандартным Include файлам
      QString                  mProjectPath;    //Путь к файлам проекта

      SvErrorList              mErrorList;      //! Список ошибок компиляции
    public:
      //----------------------------------------------------
      // Компилятор часть 1 (конструктор и инициализация)
      SrCompiler();
      virtual ~SrCompiler();

              void         Reset();

              void         InitKeyWords();
              void         ClearTables();

              /*!
                 \brief Compile Точка входа в компиляцию
                 \param fname Имя компилируемого файла

                 Выполняет компиляцию проекта начиная с головного файла.
                 При необходимости подключает и загружает другие файлы.
                 В результате строится дерево разбора в структурах компилятора и
                 формируется список ошибок.
               */
              void         Compile( const QString &fname );

    protected:
              //Распределение переменных
              int          AllocLocal( SrType *type );
              int          AllocGlobal( SrType *type );

              QString      macroExpansion( const QString mac, const QString def );


              //----------------------------------------------------
              // Компилятор часть 2 (входной поток низкого уровня)
              void         DoFile( const QString &fname, bool current );
      virtual SrSource*    CreateSource( const QString &fname, bool current );
              int          AddFile( const QString &fname );
              bool         IsEoln() const { return mPtr >= mLine.size(); }
              void         InLine();
              SrMark       mark() const;
              QString      curLine() const { return mLine; }


              //----------------------------------------------------
              // Компилятор часть 3 (поток ошибок)
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

              //----------------------------------------------------
              // Компилятор часть 4 (препроцессор)
              void         Blank();
              void         White();
              bool         Match( const char *ptr ); //Если совпадает, то съедает
              bool         Match( char ch );    //Если совпадает, то съедает
              bool         ScanName();
              bool         ScanNameImp( int &ptr );
              bool         ScanInt();

              void         DoPreprocess();
              void         DoInclude();
              void         DoExpand();
              void         DoDefine();
              void         IfLine();

              //----------------------------------------------------
              // Компилятор часть 5 (условие в препроцессоре #if)
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

              //Компилятор часть 6 (сканер)
              bool         ScanDigit();     //Проверить наличие числа и сканировать если есть
              bool         NextToken();     //Получить следующий токен
              void         NeedSemicolon(); //Проверить следующий токен на точку с запятой


              //Компилятор часть 7 (таблицы)
              //Коментарий
              void         AddAsmRemark(const QString &sour);          //Добавить коментарий, ассемблерный код
              //Таблица символов
              bool         isGlobalSymbol( const QString &name );

              //Компилятор часть 8 (разбор выражений)
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
              bool         BoWaitFunction(SrValuePtr &val );   //Реализация специальной функции srWait
              bool         BoCatchFunction(SrValuePtr &val );   //Реализация специальной функции srCatch
              bool         BoThrowFunction(SrValuePtr &val );   //Реализация специальной функции srThrow
              bool         BoExceptionFunction(SrValuePtr &val ); //Реализация специальной функции srException


              //Компилятор часть 9 (синтаксический разбор)
              void         DoCBlock();                        //Блок данных
              void         DoImport();                        //Объявление импортируемого объекта
              void         DoDeclareGlobal();                 //Объявление или определение переменных и функций
              void         DoStructure();                     //Определение структуры
              void         DoNewFunction(SrFunction *fun );   //Объявление/определение новой функции
              void         DoParamList(SrFunction *fun );     //Декодирование списка параметров
              void         DoParamAddress( SrFunction *fun ); //Назначить параметрам адреса
              //Определение подтипа
              void         DoSubType( SrType *type, SrVariablePtr &var, SrFunctionPtr &fun, int *addressCount );
              SrType*      DoBaseType();                      //Определение основного типа
              SrOperator*  DoStatement(SrOperator *parent);   //Оператор
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

              //Компилятор часть A вычисление константных выражений
              void         constValueCalc( SrValue *val );
              void         constValueLongCalc( SrValueBinaryLong *val );



              //Компилятор часть B листинг
              QString      Listing();

    };

}

#endif //SRCOMPILER_H

