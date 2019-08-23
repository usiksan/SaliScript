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
#ifndef SVCOMPILER_H
#define SVCOMPILER_H

#include "SvHost/SvProgramm.h"

#include <QStack>
#include <QHash>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QDebug>

#define TVERSION "V6.06"
#define Assert(x)
#define TFUN_FRAME_SIZE     3  //Размер фрейма функции
#define SV_OPER_MAX        32  //Максимальное количество операндов в одной бинарной операции
#define SV_FUN_PARAM_MAX   32  //Максимальное количество параметров в вызове функции

namespace SvCompiler6
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
     \brief The SvSort enum Вид размещения переменной.
     Переменная может быть размещена в глобальном пространстве,
     в локальном пространстве (в стеке) и быть членом какой-либо структуры.
   */
  enum SvSort {
    tsrGlobal, //! Переменная размещена в глобальном пространстве
    tsrLocal,  //! Переменная размещена в локальном пространстве
    tsrMember  //! Переменная является членом структуры
    };







  //Ссылка на элемент помощи
  /*!
     \brief The SvHelp struct Помощь по объекту.

     При своей работе компилятор собирает по исходному коду коментарии
     расположенные в непосредственной близости от определения функций,
     переменных и макросов. Эти коментарии составляют быструю помощь по
     элементу и используются системой проектирования IDE для написания
     и отладки программ rc++.
   */
  struct SvHelp {
      QString         mBrief;         //! Короткое описание
      QString         mReference;     //! Ссылка на документацию
      SvMark          mDefinePos;     //! Позиция определения

      //Конструктор пустой помощи
      SvHelp();

      SvHelp( const SvHelp &hlp );

      void update( const SvHelp &hlp );

      void clear();
    };



  /*!
     \brief The SvMacro struct Макрорасширение для макроопределений вместе
     с дополнительной информацией

     Данная структура хранит макрорасширение для макроса, а также дополнительную информацию.
     Метка помощи предназначена для показа IDE. Список использованных мест
     также предназначен для IDE. На основе этого списка может быть осуществлена автозамена
     объекта, а также локализованы места его использования
   */
  struct SvMacro {
      SvHelp     mHelp;          //! Помощь по макросу
      QString    mExpander;      //! Строка с макрорасширением
      SvMarkList mReferneceList; //! Список мест, в которых используется макрос
    };

  typedef SvMacro *SvMacroPtr;
  typedef QHash<QString,SvMacroPtr> SvMacroPtrTable;


  struct SvValue;
  typedef SvValue *SvValuePtr;
  typedef QList<SvValuePtr> SvValuePtrList;




  /*!
     \brief The SvValueList struct Список значений SvValue, которые представляют
     собой ветки дерева разбора выражений.
   */
  struct SvValueList {
      SvValuePtrList mList;

      SvValueList() : mList() {}

      ~SvValueList() { qDeleteAll(mList); }

      int     addExpression( SvValue *val ) { mList.append( val ); return mList.count() - 1; }

      void    clear() { qDeleteAll(mList); mList.clear(); }

      QString listing();
    };






  struct SvType;
  typedef SvType *SvTypePtr;


  /*!
     \brief The SvVariable struct Переменная в скрипте
   */
  struct SvVariable {
      SvHelp           mHelp;               //! Помощь по переменной
      QString          mName;               //! Имя переменной
      SvType          *mType;               //! Тип переменной
      SvSort           mSort;               //! Сорт (область) переменной: глобальная, локальная, член
      int              mAddress;            //! Адрес начала относительный
      int              mArraySize;          //! Размер массива (для массивов)
      SvMark           mMarkDefine;         //! Место определения
      QString          mRemark;             //! Описание переменной
      SvValue         *mInit;               //! Инициализационное значение
      SvMarkList       mReferenceList;      //! Список используемых переменной мест


      SvVariable();
      SvVariable( const SvMark &src, const QString name, SvType *type, int addr, int arraySize = 0 );
      SvVariable( const SvVariable *src );

      ~SvVariable();

      //Добавить помощь
      void      setRemark( const QString &prevRemark, const QString &afterRemark );

      //Получить размер переменной
      int       getSize();

      //Листинг
      QString   printListing();
    };

  typedef SvVariable *SvVariablePtr;
  typedef QList<SvVariablePtr> SvVariablePtrList;
  typedef QHash<QString,SvVariablePtr> SvVariablePtrHash;



  //Список переменных
  /*!
     \brief The SvVariableList struct Список переменных. Содержит собственно список переменных,
     хэш-таблицу для быстрого поиска переменных и сорт размещения переменных
   */
  struct SvVariableList {
      SvVariablePtrList mList; //! Последовательный список переменных
      SvVariablePtrHash mHash; //! Хэш-таблица быстрого поиска переменных
      SvSort            mSort; //! Вид таблицы переменных

      SvVariableList( SvSort sort );
      ~SvVariableList();

      //Очистить список
      void          clear();

      //Добавить уже созданную переменную
      void          addVariable( SvVariable *var );

      //Проверить наличие переменной в списке
      bool          isPresent( const QString name ) const { return mHash.contains( name ); }

      //Добавить в список копию из заданного списка
      void          addList( const SvVariableList &src );

      //Получить переменную
      SvVariable*   getVariable( const QString name ) { return mHash.value( name, 0 ); }

      //Получить листинг переменных
      QString       listing( int level );
    };






  struct SvOperatorBlock;

  /*!
     \brief The SvFunction struct Описывает функцию в глобальном контексте
   */
  struct SvFunction {
      SvHelp           mHelp;               //! Помощь по функции
      QString          mName;               //! Имя функции
      bool             mDefined;            //! Флаг, выставляется, когда тело функции уже определено
      bool             mDeclared;           //! Флаг, выставляется, когда функция объявлена (например в классе)
                                            //! в глобальном контексте все функции объявлены по умолчанию
      int              mAddress;            //! Адрес начала кода
      int              mImportIndex;        //! Индекс импортной функции
      int              mParamSize;          //! Размер области параметров
      SvMark           mMarkDefine;         //! Место определения
      SvMark           mMarkDeclare;        //! Место первого объявления
      QString          mRemark;             //! Описание переменной
      SvVariableList   mParams;             //! Список параметров функции
      SvType          *mResultType;         //! Тип результата
      SvType          *mType;               //! Тип данной функции
      SvOperatorBlock *mBody;               //! Тело функции
      int              mLocalAmount;        //! Место под локальные переменные
      SvMarkList       mReferenceList;      //! Список мест использования функции

      SvFunction();
      SvFunction( const QString &name, SvType *result, const SvMark &mark );

      int         resultOffset() const;

      //Получить глобальное имя функции
      QString     globalName() const;

      SvVariable* getParam( const QString name ) const { return mParams.mHash.value( name, 0 ); }

      //Добавить параметр, если возможно
      bool        addParam( SvVariable *param );

      //Установить комментарии к функции
      void        setRemark(  const QString &prevRemark, const QString &afterRemark );

      //Сформировать листинг объявления функции
      QString     listingDeclaration();

      //Сформировать листинг определения функции
      QString     listingDefinition();
    };

  typedef SvFunction *SvFunctionPtr;
  typedef QList<SvFunctionPtr> SvFunctionPtrList;
  typedef QHash<QString,SvFunctionPtr> SvFunctionPtrHash;





  /*!
     \brief The SvFunctionList struct Список функций. Содержи собственно список функций,
     а также хэш-таблицу для быстрого доступа к функции
   */
  struct SvFunctionList {
      SvFunctionPtrList mList; //Список функций контекста
      SvFunctionPtrHash mHash; //Хэш-функций

      SvFunctionList();
      ~SvFunctionList();

      //Очистить список
      void        clear();

      //Добавить функцию к списку
      void        addFunction(SvFunctionPtr fun , const QString name);

      //Заменить функцию в списке
      SvFunction* addFunctionDefinition( SvFunctionPtr fun );

      //Проверить наличие имени среди функций
      bool        isPresent( const QString name ) const { return mHash.contains(name); }

      //Получить функцию
      SvFunction* getFunction( const QString name ) { return mHash.value( name, nullptr ); }

      //Сформировать листинг описаний функций
      QString     listingDeclaration();

      //Сформировать листинг функций с определениями
      QString     listingDefinition();
    };









  struct SvStruct;
  struct SvFunctionType;
  class SvTypeList;

  /*!
     \brief The SvType struct Основной и производный тип

     Все объекты скрипта имеют какой-либо тип. При инициализации
     компилятор создает и регистрирует набор базовых типов. Из этих базовых
     типов может создаваться новый тип.

     Имя типа - это то имя, которым тип представлен в программе.

     Сигнатура типа формируется из сигнатуры базового типа путем добавления модификаторов.
     Сигнатура нужна для определения одинаковых типов, но имеющих разные имена. Это актуально
     для typedef-ов (в настоящее время не используются) и для функций. Функции одинаковые по
     параметрам и по результату будут иметь одинаковую сигнатуру.
   */
  struct SvType {
      QString            mName;           //! Первое имя типа
      QString            mSignature;      //! Сигнатура типа
      int                mClass;          //! Основной тип (класс)
      int                mNumElem;        //! Количество элементов массива
      int                mSize;           //! Размер объекта данного типа
      SvType            *mBaseType;       //! Базовый тип для производных типов
      SvHelp             mHelp;           //! Помощь по типу
      SvTypeList        *mTypeList;       //! Список всех типов для генерации новых типов из данного
                                          //! Этот список содержится в компиляторе и предоставляется
                                          //! единый на весь компилятор.

      //Конструктор по умолчанию
      SvType();

      //Деструктор по умолчанию
      virtual ~SvType();

      //Конструктор типа с именем, классом, размером и подтипом
      SvType(const QString &name, int svClass, int size, SvType *base );

      //Получить id объекта
      int             objectId() const { return (mClass >> 20) & 0xff; }

      //Сконструировать тип-объект
      void            createObject( int i );

      //Получить тип как структуру (класс)
      SvStruct        *toStruct();

      //Получить тип как функцию
      SvFunctionType *toFunction();

      //Получить сигнатуру от указателя на данный тип
      QString         signaturePointer() const;

      //Получить сигнатуру на массив элементов данного типа
      QString         signatureArray() const;

      //Получить указатель на тип
      SvType         *getTypePointer();

      //Получить тип массив
      SvType         *getTypeArray(int numElem);

      bool            isInt()      const { return mClass == CLASS_INT; }
      bool            isVoid()     const { return mClass == CLASS_VOID; }
      bool            isArray()    const { return mClass == CLASS_ARRAY; }
      bool            isPointer()  const { return mClass == CLASS_POINTER; }
      bool            isCBlock()   const { return mClass == CLASS_CBLOCK; }
      bool            isCString()  const { return mClass == CLASS_CSTRING; }
      bool            isStruct()   const { return mClass == CLASS_STRUCT; }
      bool            isFunction() const { return mClass == CLASS_FUNCTION; }

      //Проверить возможность назначения типов
      //т.е. может ли тип src быть присвоен данному типу
      //Проверить соответствие параметра
      bool            isMatchParam(SvType *src , bool srcNull);

      //Построить сигнатуру
      virtual QString buildSignature();

      //Построить листинг
      virtual QString listing();

    };






  typedef QList<SvTypePtr> SvTypePtrList;
  typedef QHash<QString,SvTypePtr> SvTypePtrHash;



  /*!
     \brief The SvTypeList class Список типов. Содержит список всех типов, а также
     две хэш-таблицы для быстрого доступа к типам. Одна из таблиц производит
     выборку по именам типов, а другая - по сигнатурам.

   */
  class SvTypeList {
      SvTypePtrHash mHash; //Типы с именами
      SvTypePtrHash mSign; //Типы с сигнатурами
    public:
      SvTypePtrList mList; //Список всех типов

      SvTypeList();
      ~SvTypeList();

      //Очистить список типов
      void    clear();

      //Проверить наличие типа с заданным именем
      bool    isPresent( const QString &name ) const { return mHash.contains(name); }

      //Получить тип по имени
      SvType* getType( const QString &name ) { return mHash.value( name, nullptr ); }

      //Получить тип по сигнатуре
      SvType* getTypeBySignature( const QString &sign ) const { return mSign.value( sign, nullptr ); }

      //Получить указатель на тип
      SvType* getTypePointer( SvType *base );

      //Получить тип массив
      SvType* getTypeArray(SvType *base, int numElem);

      //Добавить тип к списку
      SvType* addType( SvType *type );

      //Добавить имя для типа
      void    addTypeName( SvType *type, const QString &name );

      //Сформировать листинг типов
      QString listing();
    };








  /*!
     \brief The SvFunParam struct Формальный параметр функции в описании типа функции
   */
  struct SvFunParam {
      QString          mName;               //! Формальное имя параметра
      SvType          *mType;               //! Тип параметра

      SvFunParam() : mName(), mType(nullptr) {}

      SvFunParam( SvVariable *var ) : mName(var->mName), mType(var->mType) {}

      SvFunParam& operator = ( const SvFunParam &p ) { mName = p.mName; mType = p.mType; return *this; }
    };

  typedef QList<SvFunParam> SvParamList;




  /*!
     \brief The SvFunctionType struct Тип описывающий функцию. В дополнение к базовому типу
     в типе-функции присутствует список формальных параметров и результат.
   */
  struct SvFunctionType : public SvType {
      SvParamList      mParamList;      //Список параметров
      SvType          *mResult;         //Тип результата
      int              mParamSize;      //Размер области параметров

      SvFunctionType() : mParamList(), mResult(nullptr), mParamSize(0) { mClass = CLASS_FUNCTION; }
      ~SvFunctionType() override;

      //Построить сигнатуру
      virtual QString buildSignature() override;
    };






  /*!
     \brief The SvStruct struct Тип описывающий структуру. В дополнение к базовому типу
     в типе-структуре присутствует список членов и указатель на наследованную структуру.
   */
  struct SvStruct : public SvType {
      bool               mDefined;        //! Структура определена
      SvStruct          *mBaseStruct;     //! Базовая структура, от которой наследовалась данная
      SvVariableList     mMemberList;     //! Список всех членов. При создании структуры в этот
                                          //! спислк копируется список членов из наследовуемой
                                          //! структуры

      //Конструктор обычного класса
      SvStruct( const QString name );
      ~SvStruct() override;

      //Для удобства использования
      bool            isMember( const QString name ) { return mMemberList.mHash.contains(name); }
      SvVariable     *getMember( const QString name ) { return mMemberList.mHash.value(name, nullptr); }

      void            setBase(SvStruct *base );

      void            addMember( SvVariable *mem );

      void            addFunction( SvFunction *fun );

      //Построить листинг
      virtual QString listing() override;
    };









  /*!
     \brief The SvSource class Источник исходного кода.

     При компиляции из отдельной программы скрипт, как правило, читается
     из файла. Поэтому по умолчанию при обнаружении ссылки на какой-либо
     файл компилятор открывает его с диска. Однако, при использовании
     IDE может быть полезным получать содержимое файла не с диска, а
     прямо из редактора. В этом случае соответствующие функции компилятора должны
     быть переопределены.
   */
  class SvSource {
    protected:
      int          mLineCount;   //! Счетчик строк исходного файла
      int          mFileId;      //! Идентификатор файла в общей таблице файлов
      QTextStream *mInputStream; //! Исходный код в виде потока
    public:

      SvSource( int fileId ) : mLineCount(0), mFileId(fileId), mInputStream(nullptr) {}
      virtual ~SvSource();
      QString ReadLine();
      bool    Eof();
      int     Line() const { return mLineCount; }
      int     FileId() const { return mFileId; }
    };



  //Указатель на источник данных
  typedef SvSource *SvSourcePtr;



  /*!
     \brief The SvSourceFile class Источник исходного кода в виде файла на диске.
   */
  class SvSourceFile : public SvSource {
      QFile       *mFile;
    public:
      SvSourceFile( QFile *file, int fileId );
      ~SvSourceFile();
    };





  /*!
     \brief The SvStatement enum Коды идентификации операторов

     Компилятор может выполнять некоторую оптимизацию в зависимости от
     порядка следования операторов.
   */
  enum SvStatement {
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
     \brief The SvValueCode enum Коды идентификации операций в дереве разбора выражений
   */
  enum SvValueCode {
    svvVariable,        //Переменная (глобальная, локальная, параметр)
    svvFunction,        //Функция (глобальная)
    svvWaitFun,         //Специальная функция svWait
    svvThrowFun,        //Специальная функция svThrow
    svvCatchFun,        //Специальная функция svCatch
    svvExceptionFun,    //Специальная функция svException
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
     \brief The SvValue struct Ветка и лист в дереве разбора выражений

     Все выражения представлены экземпляром структуры SvValue. В зависимости
     от фактической наследованной структуры SvValueXXX выражение представляется
     в виде дерева.
   */
  struct SvValue {
    protected:
      SvType      *mType;        //Тип значения, полученного в результате
    public:
      SvMark       mMark;        //Место в файле для данного значения
      int          mConstInt;    //Вычисленный результат
      bool         mConst;       //Флаг показывает константный результат

      SvValue( SvType *type, const SvMark &mark );
      virtual ~SvValue() {}

      void    clear() { mType = nullptr; mMark.clear(); mConstInt = 0; mConst = false; }

      //Проверка константного значения
      bool    isConst() const { return mConst; }

      //Преобразовать к значению условия
      bool    toCondition() const { return mConstInt; }

      //Получить целую константу
      int     toConstInt() const { return mConstInt; }


      //Получить строку листинга
      virtual QString     listing() = 0;

      //Получить код операции
      virtual SvValueCode code() const = 0;

      //Получить тип операции
      virtual SvType*     getType() { return mType; }

      /*!
        \brief setType Установить новый тип для значения
        \param tp Новый тип для значения
      */
              void        setType( SvType *tp ) { mType = tp; }

      //Получить класс типа результата
              int         getClass() { return getType() == nullptr ? 0 : mType->mClass; }
  };



  struct SvValueError : SvValue {
      SvValueError( SvType *type, const SvMark &mark ) : SvValue( type, mark ) {}

      //Получить строку листинга
      virtual QString     listing() override { return QString(";Error"); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLast; }
    };




  /*!
     \brief The SvValueVariable struct Переменная (глобальная, локальная, параметр)
   */
  struct SvValueVariable : SvValue {
      SvVariable *mVariable;
      int         mAddonAddress;

      SvValueVariable(SvVariable *var, const SvMark &mark );

      //Получить строку листинга
      virtual QString     listing() override { if( mVariable ) return QString(" <%1> ").arg(mVariable->printListing()); return QString("<ERR>"); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvVariable; }
    };




  /*!
     \brief The SvValueFunction struct Взятие адреса функции (глобальной)
   */
  struct SvValueFunction : SvValue {
      SvFunction *mFunction;

      SvValueFunction( SvFunction *fun, const SvMark &mark ) :
        SvValue( fun->mType, mark ), mFunction(fun) { }

      //Получить строку листинга
      virtual QString     listing() override { if( mFunction ) return QString(" <%1> ").arg( mFunction->listingDeclaration() ); return QString("<ERR>"); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvFunction; }
    };





  /*!
     \brief The SvValueWaitFun struct Специальная функция svWait
   */
  struct SvValueWaitFun : SvValue {
      SvValueWaitFun( SvType *voidType, const SvMark &mark ) : SvValue( voidType, mark) { }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" svWait() "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvWaitFun; }
    };


  /*!
     \brief The SvValueThrowFun struct Специальная функция svThrow
   */
  struct SvValueThrowFun : SvValue {
      SvValue *mThrowCode;

      SvValueThrowFun( SvValue *throwCode, SvType *voidType, const SvMark &mark ) : SvValue( voidType, mark), mThrowCode(throwCode) { }
      ~SvValueThrowFun() { delete mThrowCode; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" svThrow(%1) ").arg( mThrowCode->listing() ); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvThrowFun; }
    };


  /*!
     \brief The SvValueCatchFun struct Специальная функция svCatch
   */
  struct SvValueCatchFun : SvValue {
      SvValue *mCatchMask;

      SvValueCatchFun( SvValue *catchMask, SvType *voidType, const SvMark &mark ) : SvValue( voidType, mark ), mCatchMask(catchMask) { }
      ~SvValueCatchFun() { delete mCatchMask; }

      //Получить строку листинга
      virtual QString     listing() override { return QString(" svCatch(%1) ").arg( mCatchMask->listing() ); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvCatchFun; }
    };



  /*!
     \brief The SvValueExceptionFun struct Специальная функция svException
   */
  struct SvValueExceptionFun : SvValue {
      SvValueExceptionFun( SvType *intType, const SvMark &mark ) : SvValue( intType, mark ) {}

      //Получить строку листинга
      virtual QString     listing() override { return QString(" svException() "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvExceptionFun; }
    };




  /*!
     \brief The SvValueMemberVariable struct Член от произвольной структуры
   */
  struct SvValueMemberVariable : SvValueVariable {
      SvValue *mStruct; //Выражение вычисления структуры

      SvValueMemberVariable( SvVariable *var, SvValue *struc, const SvMark &mark );
      ~SvValueMemberVariable() override;

      //Получить строку листинга
      virtual QString     listing() override { if(mStruct) return mStruct->listing() + QString(".") + SvValueVariable::listing(); return QString("ERR.") + SvValueVariable::listing(); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvMemberVariable; }
    };







  /*!
     \brief The SvValueConstInt struct Константа целая
   */
  struct SvValueConstInt : SvValue {
      SvValueConstInt( int val, SvType *intType, const SvMark &mark ) : SvValue( intType, mark ) { mConst = true; mConstInt = val; }

      //Получить строку листинга
      virtual QString     listing() override { return QString::number(mConstInt); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvConstInt; }
    };




  /*!
     \brief The SvValueConstString struct Постоянная строка
   */
  struct SvValueConstString : SvValue {
      int mIndex; //Индекс строки в таблице строк

      SvValueConstString( int index, SvType *strType, const SvMark &mark ) : SvValue( strType, mark ), mIndex(index) {}

      //Получить строку листинга
      virtual QString     listing() override { return QString("cstring[%1]").arg(mIndex); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvConstString; }
    };





  /*!
     \brief The SvValueUnary struct Унарная операция
   */
  struct SvValueUnary : public SvValue {
      SvValue *mOperand;

      SvValueUnary( SvValue *oper, const SvMark &mark ) :
        SvValue( oper->getType(), mark ), mOperand(oper) {}
      ~SvValueUnary() override { if( mOperand ) delete mOperand; }

      //Получить строку листинга
      virtual QString     listing() override { if( mOperand ) return operation().arg( mOperand->listing() ); return operation().arg("ERR"); }

      //Получить значение операции
      virtual QString     operation() const = 0;

      //Результат константной операции
      virtual int         constOperation( int val ) { Q_UNUSED(val) return 0; }

      //Вычислить константу
      void                checkConst() { mConst = mOperand && mOperand->mConst; if( mConst ) mConstInt = constOperation( mOperand->mConstInt ); }
    };




  /*!
     \brief The SvValuePointer struct Разименовать указатель
   */
  struct SvValuePointer : public SvValueUnary {
      SvValuePointer( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {
          mType = mOperand->getType();
          if( mType )
            mType = mType->mBaseType;
          }

      //Получить значение операции
      virtual QString     operation() const override { return QString(" *(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvPointer; }

    };




  /*!
     \brief The SvValueAddress struct Взять адрес
   */
  struct SvValueAddress : public SvValueUnary {
      SvValueAddress( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {
          mType = mOperand->getType();
          if( mType )
            mType = mType->getTypePointer();
          }

      //Получить значение операции
      virtual QString     operation() const override { return QString(" &(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvAddress; }
    };





  /*!
     \brief The SvValuePredInc struct Прединкремент
   */
  struct SvValuePredInc : public SvValueUnary {
      SvValuePredInc( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ++(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvPredInc; }
    };





  /*!
     \brief The SvValuePredDec struct Преддекремент
   */
  struct SvValuePredDec : public SvValueUnary {
      SvValuePredDec( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" --(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvPredDec; }
    };





  /*!
     \brief The SvValuePostInc struct Постинкремент
   */
  struct SvValuePostInc : public SvValueUnary {
      SvValuePostInc( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)++ "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvPostInc; }
    };





  /*!
     \brief The SvValuePostDec struct Постдекремент
   */
  struct SvValuePostDec : public SvValueUnary {
      SvValuePostDec( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)-- "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvPostDec; }
    };





  /*!
     \brief The SvValueBitNot struct Битовое отрицание
   */
  struct SvValueBitNot : public SvValueUnary {
      SvValueBitNot( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ~(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvNot; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return ~val; }
    };





  /*!
     \brief The SvValueLogNot struct Логическое отрицание
   */
  struct SvValueLogNot : public SvValueUnary {
      SvValueLogNot( SvValue *oper, SvType *intType, const SvMark &mark ) :
        SvValueUnary( oper, mark ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const override { return QString(" !(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLogNot; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return !val; }
    };





  /*!
     \brief The SvValueNeg struct Изменение знака
   */
  struct SvValueNeg : public SvValueUnary {
      SvValueNeg( SvValue *oper, const SvMark &mark ) :
        SvValueUnary( oper, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" -(%1) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvNeg; }

      //Результат константной операции
      virtual int    constOperation( int val ) override { return -val; }
    };







  /*!
     \brief The SvValueBinary struct Бинарная операция
   */
  struct SvValueBinary : public SvValue {
      SvValue *mOperand1;
      SvValue *mOperand2;

      SvValueBinary( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValue( oper1->getType(), mark ), mOperand1(oper1), mOperand2(oper2) {}
      ~SvValueBinary() override { if( mOperand1 ) delete mOperand1; if( mOperand2 ) delete mOperand2; }


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
    };





  /*!
     \brief The SvValueStore struct Операция сохранения
   */
  struct SvValueStore : public SvValueBinary {
      SvValueStore( SvValue *oper1, const SvMark &mark ) :
        SvValueBinary( oper1, mark, nullptr ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) = (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvStore; }
    };





  /*!
     \brief The SvValueArrayCell struct Операция индекса массива
   */
  struct SvValueArrayCell : public SvValueBinary {
      SvValueArrayCell( SvValue *array, const SvMark &mark, SvValue *index = nullptr ) :
        SvValueBinary( array, mark, index ) {
          mType = mOperand1->getType();
          if( mType )
            mType = mType->mBaseType;
          }

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1)[(%2)] "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvArrayCell; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 + val2 * mType->mBaseType->mSize; }
    };





  /*!
     \brief The SvValueMul struct Умножение
   */
  struct SvValueMul : public SvValueBinary {
      SvValueMul( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) * (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvMul; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 * val2; }
    };





  /*!
     \brief The SvValueMulStore struct Умножение и сохранение
   */
  struct SvValueMulStore : public SvValueBinary {
      SvValueMulStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) *= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvMulStore; }
    };





  /*!
     \brief The SvValueDiv struct Деление
   */
  struct SvValueDiv : public SvValueBinary {
      SvValueDiv( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) / (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvDiv; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val2 != 0 ? val1 / val2 : 0; }
    };





  /*!
     \brief The SvValueDivStore struct Деление и сохранение
   */
  struct SvValueDivStore : public SvValueBinary {
      SvValueDivStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) /= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvDivStore; }
    };






  /*!
     \brief The SvValueMod struct Остаток от деления
   */
  struct SvValueMod : public SvValueBinary {
      SvValueMod( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) mod (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvMod; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val2 != 0 ? val1 % val2 : 0; }
    };






  /*!
     \brief The SvValueModStore struct Остаток от деления и сохранение
   */
  struct SvValueModStore : public SvValueBinary {
      SvValueModStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) mod= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvModStore; }
    };






  /*!
     \brief The SvValueAdd struct Сложение
   */
  struct SvValueAdd : public SvValueBinary {
      SvValueAdd( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) + (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvAdd; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 + val2; }
    };






  /*!
     \brief The SvValueAddStore struct Сложение и сохранение
   */
  struct SvValueAddStore : public SvValueBinary {
      SvValueAddStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) += (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvAddStore; }
    };





  /*!
     \brief The SvValueSub struct Вычитание
   */
  struct SvValueSub : public SvValueBinary {
      SvValueSub( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) - (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvSub; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 - val2; }
    };






  /*!
     \brief The SvValueSubStore struct Вычитание и сохранение
   */
  struct SvValueSubStore : public SvValueBinary {
      SvValueSubStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) -= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvSubStore; }
    };





  /*!
     \brief The SvValueLShift struct Сдвиг влево
   */
  struct SvValueLShift : public SvValueBinary {
      SvValueLShift( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) << (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLShift; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 << val2; }
    };






  /*!
     \brief The SvValueLShiftStore struct Сдвиг влево и сохранение
   */
  struct SvValueLShiftStore : public SvValueBinary {
      SvValueLShiftStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) <<= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLShiftStore; }
    };





  /*!
     \brief The SvValueRShift struct Сдвиг вправо
   */
  struct SvValueRShift : public SvValueBinary {
      SvValueRShift( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) >> (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvRShift; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 >> val2; }
    };





  /*!
     \brief The SvValueRShiftStore struct Сдвиг вправо и сохранение
   */
  struct SvValueRShiftStore : public SvValueBinary {
      SvValueRShiftStore( SvValue *oper1, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) >>= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvRShiftStore; }
    };





  /*!
     \brief The SvValueOrStore struct Бинарное ИЛИ и сохранение
   */
  struct SvValueOrStore : public SvValueBinary {
      SvValueOrStore( SvValue *oper1, const SvMark &mark ) :
        SvValueBinary( oper1, mark, nullptr ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) |= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvOrStore;  }
    };




  /*!
     \brief The SvValueAndStore struct Бинарное И и сохранение
   */
  struct SvValueAndStore : public SvValueBinary {
      SvValueAndStore( SvValue *oper1, const SvMark &mark ) :
        SvValueBinary( oper1, mark, nullptr ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) &= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvAndStore; }
    };





  /*!
     \brief The SvValueXorStore struct Бинарное ХОР и сохранение
   */
  struct SvValueXorStore : public SvValueBinary {
      SvValueXorStore( SvValue *oper1, const SvMark &mark ) :
        SvValueBinary( oper1, mark, nullptr ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) ^= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvXorStore; }
    };





  /*!
     \brief The SvValueEqu struct Равенство
   */
  struct SvValueEqu : public SvValueBinary {
      SvValueEqu( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) == (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 == val2; }
    };





  /*!
     \brief The SvValueNotEqu struct Неравенство
   */
  struct SvValueNotEqu : public SvValueBinary {
      SvValueNotEqu( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) != (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvNotEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 != val2; }
    };





  /*!
     \brief The SvValueLessEqu struct Меньше или равно
   */
  struct SvValueLessEqu : public SvValueBinary {
      SvValueLessEqu( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const override { return QString(" (%1) <= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLessEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) override { return val1 <= val2; }
    };





  /*!
     \brief The SvValueLess struct Меньше
   */
  struct SvValueLess : public SvValueBinary {
      SvValueLess( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) < (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const { return svvLess; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 < val2; }
    };





  /*!
     \brief The SvValueGreat struct Больше
   */
  struct SvValueGreat : public SvValueBinary {
      SvValueGreat( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) > (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const { return svvGreat; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 > val2; }
    };





  /*!
     \brief The SvValueGreatEqu struct Больше или равно
   */
  struct SvValueGreatEqu : public SvValueBinary {
      SvValueGreatEqu( SvValue *oper1, SvType *intType, const SvMark &mark, SvValue *oper2 = nullptr ) :
        SvValueBinary( oper1, mark, oper2 ) { mType = intType; }

      //Получить значение операции
      virtual QString operation() const { return QString(" (%1) >= (%2) "); }

      //Получить код операции
      virtual SvValueCode code() const { return svvGreatEqu; }

      //Результат константной операции
      virtual int    constOperation( int val1, int val2 ) { return val1 >= val2; }
    };






  /*!
     \brief The SvValueBinaryLong struct Бинарная операция для ленивых логических операций
   */
  struct SvValueBinaryLong : public SvValue {
      SvValuePtr mOperand[SV_OPER_MAX];   //Операнды
      int        mOperCount;              //Количество операндов

      SvValueBinaryLong( SvValue *oper1, SvType *intType, const SvMark &mark );

      ~SvValueBinaryLong();

      //Получить указатель на следующий операнд
      SvValuePtr& nextOperand();

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
     \brief The SvValueLogAnd struct Логическое И
   */
  struct SvValueLogAnd : public SvValueBinaryLong {
      int    mExitLabel;
      SvValueLogAnd( SvValue *oper1, SvType *intType, const SvMark &mark ) :
        SvValueBinaryLong( oper1, intType, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" && "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLogAnd; }
    };






  /*!
     \brief The SvValueLogOr struct Логическое ИЛИ
   */
  struct SvValueLogOr : public SvValueBinaryLong {
      int    mExitLabel;
      SvValueLogOr( SvValue *oper1, SvType *intType, const SvMark &mark ) :
        SvValueBinaryLong( oper1, intType, mark ), mExitLabel(0) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" || "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvLogOr; }
    };






  /*!
     \brief The SvValueOr struct Бинарное ИЛИ
   */
  struct SvValueOr : public SvValueBinaryLong {
      SvValueOr( SvValue *oper1, SvType *intType, const SvMark &mark ) :
        SvValueBinaryLong( oper1, intType, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" | "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvOr; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) override { return val1 | val2; }
    };






  /*!
     \brief The SvValueAnd struct Бинарное И
   */
  struct SvValueAnd : public SvValueBinaryLong {
      SvValueAnd( SvValue *oper1, SvType *intType, const SvMark &mark ) :
        SvValueBinaryLong( oper1, intType, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" & "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvAnd; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) override { return val1 & val2; }

    };






  /*!
     \brief The SvValueXor struct Бинарное Исключающее ИЛИ
   */
  struct SvValueXor : public SvValueBinaryLong {
      SvValueXor( SvValue *oper1, SvType *intType, const SvMark &mark ) :
        SvValueBinaryLong( oper1, intType, mark ) {}

      //Получить значение операции
      virtual QString operation() const override { return QString(" ^ "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvXor; }

      //Результат константной операции
      virtual int     constOperation( int val1, int val2 ) override { return val1 ^ val2; }
    };






  /*!
     \brief The SvValueComma struct Операция ,
   */
  struct SvValueComma : public SvValueBinaryLong {
      SvValueComma( SvValue *oper1, const SvMark &mark ) :
        SvValueBinaryLong( oper1, oper1->getType(), mark ) { }

      //Получить значение операции
      virtual QString operation() const override { return QString(" , "); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvComma; }

      //Получить тип операции
      virtual SvType *getType() override { if( mOperand[mOperCount-1] ) return mOperand[mOperCount-1]->getType(); return mType; }
    };







  /*!
     \brief The SvValueCondition struct Тройная условная операция
   */
  struct SvValueCondition : public SvValue {
      SvValue *mCondition;
      SvValue *mTrue;
      SvValue *mFalse;
      int      mFalseAddress;
      int      mExitAddress;

      SvValueCondition( SvValue *cond, SvType *intType, const SvMark &mark ) :
        SvValue( intType, mark ), mCondition(cond), mTrue(nullptr), mFalse(nullptr), mFalseAddress(0), mExitAddress(0) {}

      ~SvValueCondition() override { if( mCondition ) delete mCondition; if( mTrue ) delete mTrue; if( mFalse ) delete mFalse; }

      //Получить строку листинга
      virtual QString listing() override { if( mCondition && mTrue && mFalse ) return QString(" (%1 ? %2 : %3) ").arg( mCondition->listing() ).arg( mTrue->listing() ).arg( mFalse->listing() ); return QString("ERR"); }

      //Получить код операции
      virtual SvValueCode code() const override { return svvCondition; }

      //Получить тип операции
      virtual SvType *getType() override { if( mTrue ) return mTrue->getType(); return mType; }
    };





  /*!
     \brief The SvValueCall struct Вызов функции
   */
  struct SvValueCall : public SvValue {
      SvValue    *mFunction;
      SvValuePtr  mParam[SV_FUN_PARAM_MAX];  //Параметры функции
      int         mParamCount;               //Количество параметров

      SvValueCall( SvValue *fun, SvType *result, const SvMark &mark );

      ~SvValueCall() override;

      SvValuePtr& nextParam();

      bool        isParamAvailable() const { return mParamCount < SV_FUN_PARAM_MAX - 1; }

      //Получить строку листинга
      virtual QString listing() override;

      //Получить код операции
      virtual SvValueCode code() const override { return svvCall; }
    };














  /*!
     \brief The SvOperator struct Базовая конструкция оператора. Все
     операторы выполняются последовательно, т.е. имеют один вход и один выход.
     Внутри некоторых операторов может быть блок других операторов.
   */
  struct SvOperator {
      SvMark      mMark;   //! Маркер начала оператора
      QString     mRemark; //! Исходная строка в начале оператора
      SvOperator *mParent; //! Родительский оператор

      SvOperator( const SvMark &src, const QString remark ) : mMark(src), mRemark(remark), mParent(nullptr) {}
      virtual ~SvOperator() {}

      //Сформировать листинг
      virtual QString     listing( int level ) = 0;

      //Получить строку с заданным количеством пробелов
      QString             padding( int level ) const { return QString(level,QChar(' ')); }

      //Получить тип операции
      virtual SvStatement statement() const = 0;
    };






  /*!
     \brief The SvOperatorReturn struct Оператор возврата из функции
   */
  struct SvOperatorReturn : public SvOperator {
      SvValue    *mResult;   //Значение возвращаемого результата
      SvFunction *mFunction; //Функция, из которой осуществляется возврат

      SvOperatorReturn( const SvMark &src, const QString remark, SvFunction *fun );
      ~SvOperatorReturn();

      //Сформировать листинг
      virtual QString     listing( int level ) override;

      //Получить тип операции
      virtual SvStatement statement() const override { return tstReturn; }

    };






  /*!
     \brief The SvOperatorContext struct Базовый оператор для операторов,
     имеющих внутри себя собственный котекст. Примером такого оператора является
     for, который позволяет определять переменные прямо внутри условия.
   */
  struct SvOperatorContext : public SvOperator {
      SvOperatorContext *mParentContext; //Родительский контекст
      SvVariableList     mVarLocal; //Локальные переменные контекста

      SvOperatorContext( const SvMark &mark, const QString remark, SvOperatorContext *parent ) :
        SvOperator(mark,remark), mParentContext(parent), mVarLocal(tsrLocal) {}

      SvVariable *getLocal( const QString &name ) {
        if( mVarLocal.isPresent(name) )
          return mVarLocal.getVariable(name);
        else if( mParentContext )
          return mParentContext->getLocal(name);
        return nullptr;
        }
    };






  typedef SvOperator *SvOperatorPtr;
  typedef QList<SvOperatorPtr> SvOperatorList;

  /*!
     \brief The SvOperatorBlock struct Блоковый оператор - последовательность других операторов
   */
  struct SvOperatorBlock : public SvOperatorContext {
      SvMark         mEnd;      //Место завершения блока в исходном файле
      SvOperatorList mList;     //Список операторов блока

      SvOperatorBlock( const SvMark &mark, const QString remark, SvOperatorContext *parent );

      ~SvOperatorBlock() override;

      //Сформировать листинг
      virtual QString     listing( int level ) override;

      //Получить тип операции
      virtual SvStatement statement() const override { return tstBlock; }
    };






  /*!
     \brief The SvOperatorExpression struct Оператор-выражение
   */
  struct SvOperatorExpression : public SvOperator {
      SvValue *mExpression;

      SvOperatorExpression( const SvMark &mark, const QString remark ) : SvOperator( mark, remark ), mExpression(nullptr) { }

      ~SvOperatorExpression() override { if( mExpression ) delete mExpression; }

      //Сформировать листинг
      virtual QString     listing( int level ) override { if( mExpression ) return padding(level) + mExpression->listing() + QString("\n"); return QString("ERR\n"); }

      //Получить тип операции
      virtual SvStatement statement() const override { return tstExpression; }

    };





  /*!
     \brief The SvOperatorIf struct Условный оператор
   */
  struct SvOperatorIf : public SvOperator {
      SvValue    *mCondition;     //! Выражение вычисления условия
      SvOperator *mTrue;          //! Оператор, который выполняется если условие истино
      SvOperator *mFalse;         //! Оператор, который выполняется если условие ложно
      int         mFalseAddress;  //! Метка для перехода на оператор mFalse
      int         mExitAddress;   //! Метка для выхода из условия (обход ветки mFalse)

      SvOperatorIf( const SvMark &mark, const QString remark ) :
        SvOperator(mark, remark), mCondition(nullptr), mTrue(nullptr), mFalse(nullptr), mFalseAddress(0), mExitAddress(0) {}

      ~SvOperatorIf() override { if(mCondition) delete mCondition; if(mTrue) delete mTrue; if(mFalse) delete mFalse; }

      //Сформировать листинг
      virtual QString     listing( int level ) override { return padding(level) + QString("if "); }

      //Получить тип операции
      virtual SvStatement statement() const override { return tstIf; }

    };





  /*!
     \brief The SvOperatorLoop struct Базовый оператор цикла
   */
  struct SvOperatorLoop : public SvOperatorContext {
      SvValue    *mCondition;          //! Выражение вычисления условия
      SvOperator *mBody;               //! Циклически исполняемый оператор
      int         mExitAddress;        //! Метка для выхода из цикла
      int         mConditionAddress;   //! Метка условия цикла

      SvOperatorLoop( const SvMark &mark, const QString remark, SvOperatorContext *parent ) :
        SvOperatorContext( mark, remark, parent ), mCondition(0), mBody(0), mExitAddress(0), mConditionAddress(0) {}

      ~SvOperatorLoop() override { if(mCondition) delete mCondition; if(mBody) delete mBody; }

      //Сформировать листинг
      virtual QString listing( int level ) override { return padding(level) + QString("loop"); }

    };




  /*!
     \brief The SvOperatorBreak struct Оператор прекращения цикла
   */
  struct SvOperatorBreak : public SvOperator {
      SvOperatorLoop *mLoop;  //! Цикл, к которому относится оператор break

      SvOperatorBreak( const SvMark &mark, const QString remark, SvOperatorLoop *loop ) :
        SvOperator(mark, remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString     listing( int level ) override { return padding(level) + QString("break"); }

      //Получить тип операции
      virtual SvStatement statement() const override { return tstBreak; }

    };




  /*!
     \brief The SvOperatorContinue struct Оператор перехода к условию цикла
   */
  struct SvOperatorContinue : public SvOperator {
      SvOperatorLoop *mLoop;  //! Цикл, к которому относится оператор continue

      SvOperatorContinue( const SvMark &mark, const QString remark, SvOperatorLoop *loop ):
        SvOperator(mark,remark), mLoop(loop) {}

      //Сформировать листинг
      virtual QString     listing( int level ) override { return padding(level) + QString("continue"); }

      //Получить тип операции
      virtual SvStatement statement() const override { return tstContinue; }

    };




  /*!
     \brief The SvOperatorWhile struct Оператор цикла while
   */
  struct SvOperatorWhile : public SvOperatorLoop {

      SvOperatorWhile( const SvMark &mark, const QString remark, SvOperatorContext *parent ) :
        SvOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SvStatement statement() const override { return tstWhile; }

    };




  /*!
     \brief The SvOperatorDoWhile struct Оператор цикла do while
   */
  struct SvOperatorDoWhile : public SvOperatorLoop {

      SvOperatorDoWhile( const SvMark &mark, const QString remark, SvOperatorContext *parent ) :
        SvOperatorLoop( mark, remark, parent ) {}

      //Получить тип операции
      virtual SvStatement statement() const override { return tstDo; }

    };




  /*!
     \brief The SvOperatorFor struct Оператор цикла for
   */
  struct SvOperatorFor : public SvOperatorLoop {
      SvValue    *mInit;
      SvValue    *mAction;

      SvOperatorFor( const SvMark &mark, const QString remark, SvOperatorContext *parent ) :
        SvOperatorLoop( mark, remark, parent ), mInit(nullptr), mAction(nullptr) {}

      ~SvOperatorFor() override { if(mInit) delete mInit; if(mAction) delete mAction; }

      //Получить тип операции
      virtual SvStatement statement() const override { return tstFor; }

    };








  /*!
     \brief The SvToken struct Одна лексическая единица (выход работы сканера)
   */
  struct SvToken {
      int       mId       : 16; //Тип конструкции
      unsigned  mUnsigned : 1;  //Истина для установленного флага беззнакового числа
      unsigned  mLong     : 1;  //Истина для установленного флага длинного числа
      union {
          quint32 mUValue;
          qint32  mIValue;
          double  mDValue;
        };
      QString   mString;
      SvType   *mType;          //Указатель на тип, если детектирован тип

      int     mFileId;
      int     mLine;
      int     mColon;

      bool operator == ( int i ) const { return mId == i; }
      operator int () const { return mId; }
    };




  //Таблица источников данных
  typedef QStack<SvSourcePtr>           SvSourceStack;

  //Таблица байтовых массивов
  typedef QList<QByteArray>             SvByteArrayTable;









  /*!
     \brief The SvCompiler class Компилятор собственной персоной

     Выполняет разбор исходного текста скрипта и формирует различные таблицы, на основе которых
     осуществляется генерация байт-кода.

     Компилятор выполнен по технологии рекурсивного спуска. Изначальными корнями уходит
     в SmallC Compiler лохматого года.

     В качестве промежуточного кода с версии 6 используется дерево. Ранее были триады.
     Дерево дает больше информации по откомпилированной программе. Оно позволяет
     проще проводить оптимизацию, а также упростило синтактический разбор.
   */
  class SvCompiler {
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
      SvMacroPtrTable          mMacroTable;     //Таблица макроимен

      int                      mVariant;        //Вариант контроллера
      int                      mVersion;        //Версия ПО контроллера
      int                      mGlobalAddress;  //Очередной свободный адрес глобальной переменной
      QStringList              mStringTable;    //Таблица строк
      SvByteArrayTable         mByteArrayTable; //Таблица байтовых массивов
      SvTypeList               mTypeList;       //Таблица типов
      SvVariableList           mVarGlobal;      //Список глобальных переменных
      SvFunctionList           mFunGlobal;      //Список глобальных функций

      int                      mLastStatement;  //Идентификатор последнего обработанного оператора
      SvFunction              *mActiveFunction; //Обрабатываемая функция
      SvStruct                *mActiveStruct;   //Обрабатываемая структура
      int                      mActiveImport;   //Обрабатывается import
      SvOperatorContext       *mContext;        //Текущий контекст
      SvOperatorLoop          *mLoop;           //Активный оператор цикла

      //Раздел таблицы ключевых слов
      QHash<QString,int>       mKeyWords;       //Список ключевых слов с токенами

      //Раздел типов
      SvType                  *mTypeInt;        //Целое
      SvType                  *mTypeIntPtr;     //Указатель на целое
      SvType                  *mTypeVoid;       //void
      SvType                  *mTypeVoidPtr;    //void ptr
      SvType                  *mTypeCString;    //Константная строка
      SvType                  *mTypeCStringPtr; //string ptr
      SvType                  *mTypeCBlock;     //Константный блок
      SvType                  *mTypeCBlockPtr;  //Указатель на константный блок
      SvType                  *mTypeFail;       //Недействительный тип

      //Раздел коментариев
      QList<QString>           mRemarkTable;    //Таблица коментариев

      SvToken                  mToken;          //Текущий токен

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
      SvCompiler();
      virtual ~SvCompiler();

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
              int          AllocLocal( SvType *type );
              int          AllocGlobal( SvType *type );

              QString      macroExpansion( const QString mac, const QString def );


              //----------------------------------------------------
              // Компилятор часть 2 (входной поток низкого уровня)
              void         DoFile( const QString &fname, bool current );
      virtual SvSource*    CreateSource( const QString &fname, bool current );
              int          AddFile( const QString &fname );
              bool         IsEoln() const { return mPtr >= mLine.size(); }
              void         InLine();
              SvMark       mark() const;
              QString      curLine() const { return mLine; }


              //----------------------------------------------------
              // Компилятор часть 3 (поток ошибок)
      virtual void         Error( const QString &err );
              void         ErrorInLine( const QString &err, int fileId, int lineId );
              void         errorInLine( const QString &err, const SvMark &mark );
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
              SvType*      ConstExpression(int *intRes);
              void         BExpression( SvValuePtr &val );
              void         B0( SvValuePtr &res );
              void         B1( SvValuePtr &val ); // Операция Условие
              void         B2( SvValuePtr &val ); // ||
              void         B3( SvValuePtr &val ); // &&
              void         B4( SvValuePtr &val ); // |
              void         B5( SvValuePtr &val ); // ^
              void         B6( SvValuePtr &val ); // &
              void         B7( SvValuePtr &val ); // == и !=
              void         B8( SvValuePtr &val ); // <=  >=  <  >
              void         B9( SvValuePtr &val ); // << и >>
              void         B10( SvValuePtr &val ); // + и -
              void         B11( SvValuePtr &val ); // *  /  %
              void         B12( SvValuePtr &val ); // ++val --val ~val !val -val +val *val &val val++ val--
              void         B13( SvValuePtr &val ); // [val] val()
              void         B14( SvValuePtr &val ); // val. val->
              void         B14Member( SvValuePtr &val );
              void         B15(SvValuePtr &val ); // (val) val()
              bool         BoWaitFunction(SvValuePtr &val );   //Реализация специальной функции svWait
              bool         BoCatchFunction(SvValuePtr &val );   //Реализация специальной функции svCatch
              bool         BoThrowFunction(SvValuePtr &val );   //Реализация специальной функции svThrow
              bool         BoExceptionFunction(SvValuePtr &val ); //Реализация специальной функции svException


              //Компилятор часть 9 (синтаксический разбор)
              void         DoCBlock();                        //Блок данных
              void         DoImport();                        //Объявление импортируемого объекта
              void         DoDeclareGlobal();                 //Объявление или определение переменных и функций
              void         DoStructure();                     //Определение структуры
              void         DoNewFunction(SvFunction *fun );   //Объявление/определение новой функции
              void         DoParamList(SvFunction *fun );     //Декодирование списка параметров
              void         DoParamAddress( SvFunction *fun ); //Назначить параметрам адреса
              //Определение подтипа
              void         DoSubType( SvType *type, SvVariablePtr &var, SvFunctionPtr &fun, int *addressCount );
              SvType*      DoBaseType();                      //Определение основного типа
              SvOperator*  DoStatement(SvOperator *parent);   //Оператор
              SvOperator*  DoLocal();                         //Оператор определения локальных переменных
              SvOperatorBlock *DoCompound();                      //Оператор блок
              SvOperator*  DoIf();                            //Оператор if
              SvOperator*  DoWhile();                         //Оператор while
              SvOperator*  DoDo();                            //Оператор do
              SvOperator*  DoFor();                           //Оператор for
              SvOperator*  DoSwitch();                        //Оператор switch
              SvOperator*  DoCase();                          //Оператор case
              SvOperator*  DoDefault();                       //Оператор default
              SvOperator*  DoReturn();                        //Оператор return
              SvOperator*  DoBreak();                         //Оператор break
              SvOperator*  DoContinue();                      //Оператор continue
              SvOperator*  DoExpression();                    //Оператор выражение

              //Компилятор часть A вычисление константных выражений
              void         constValueCalc( SvValue *val );
              void         constValueLongCalc( SvValueBinaryLong *val );



              //Компилятор часть B листинг
              QString      Listing();

    };

}

#endif //SVCOMPILER_H

