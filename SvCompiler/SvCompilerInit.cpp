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

    Реализация класса TCompiler часть 1 (конструктор и инициализация)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;

/*
Конструктор по умолчанию
*/
SvCompiler::SvCompiler() :
  //Раздел источников данных компиляции
  mSourceStack(),    //Стек источников данных
  mFileTable(),      //Список используемых файлов
  mFileIdCount(0),   //Счетчик для генерации идентификаторов файлов
  mFileId(0),
  mLineIndex(0),

  //Раздел обработки исходного кода
  mLine(),            //Обрабатываемая строка
  mPtr(0),            //Главный указатель кода (индекс в mLine)
  mEof(false),        //Главный флаг завершения кода
  mInLine(),          //Место для хранения входной строки
  mMacroLine(),       //Входная строка после макрорасширений
  mCRemark(true),     //Флаг выдачи С кода в качестве коментария
  mIfLevel(0),        //Текущий уровень условной компиляции
  mSkipLevel(0),      //Уровень пропускаемой условной компиляции
  mName(),            //Для сканирования имен
  mMacroTable(),      //Таблица макроимен

  mVarGlobal(tsrGlobal),

  mLastStatement(0),        //Идентификатор последнего обработанного оператора
  mActiveFunction(nullptr), //Обрабатываемая функция
  mActiveStruct(nullptr),   //Обрабатываемая структура
  mActiveImport(0),         //Обрабатывается import
  mContext(nullptr),        //Текущий контекст
  mLoop(nullptr),           //Активный оператор цикла

  //Раздел таблицы символов
  mKeyWords(),       //Список ключевых слов с токенами


  //Раздел типов
  mTypeInt(nullptr),        //Целое
  mTypeIntPtr(nullptr),
  mTypeVoid(nullptr),
  mTypeVoidPtr(nullptr),
  mTypeCString(nullptr),
  mTypeCStringPtr(nullptr),
  mTypeCBlock(nullptr),
  mTypeCBlockPtr(nullptr),
  mTypeFail(nullptr),

  //Раздел коментариев
  mRemarkTable(),     //Таблица коментариев

  mToken(),           //Текущий токен

  //Распределение памяти
  mLocalAddress(0),   //Очередной свободный адрес локальной переменной
  mLocalMax(0)       //Максимальный адрес временных переменных внутри функции

  {
  //Набрать ключевые слова
  InitKeyWords();
  //Сбросить компилятор в начальное состояние
  Reset();
  }

SvCompiler::~SvCompiler() {
  ClearTables();
  }

void
SvCompiler::Reset() {
  //Очистить все таблицы
  ClearTables();

  //Раздел источников данных компиляции
  mSourceStack.clear();    //Стек источников данных
  mFileTable.clear();      //Список используемых файлов
  mFileIdCount = 0;        //Счетчик для генерации идентификаторов файлов
  mFileId = 0;
  mLineIndex = 0;

  //Раздел обработки исходного кода
  mLine.clear();           //Обрабатываемая строка
  mPtr = 0;                //Главный указатель кода (индекс в mLine)
  mEof = false;            //Главный флаг завершения кода
  mInLine.clear();         //Место для хранения входной строки
  mMacroLine.clear();      //Входная строка после макрорасширений
  mIfLevel = 0;            //Текущий уровень условной компиляции
  mSkipLevel = 0;          //Уровень пропускаемой условной компиляции
  mName.clear();           //Для сканирования имен

  //Встроенный макрос для условной компиляции раздела импорта
  mMacroTable.insert( QString("VPU_COMPILER"), new SvMacro() );

  mStringTable.append( QString() ); //По индексу 0 добавить пустую строку



  mLastStatement = 0;            //Идентификатор последнего обработанного оператора
  mActiveFunction = nullptr;     //Обрабатываемая функция
  mActiveStruct = nullptr;       //Обрабатываемая структура
  mActiveImport = 0;             //Обрабатывается import
  mContext = nullptr;            //Текущий контекст
  mLoop = nullptr;               //Активный оператор цикла


  //Раздел типов
  //Создать типы по умолчанию
  mTypeList.addType( mTypeInt        = new SvType("int", CLASS_INT, 1, nullptr) );  //Целое
  mTypeList.addType( mTypeIntPtr     = new SvType("*int", CLASS_POINTER, 1, mTypeInt ) );
  mTypeList.addType( mTypeCString    = new SvType("cstring", CLASS_CSTRING, 1, nullptr ) );
  mTypeList.addType( mTypeCStringPtr = new SvType("*cstring", CLASS_POINTER, 1, mTypeCString ) );
  mTypeList.addType( mTypeVoid       = new SvType("void", CLASS_VOID, 0, nullptr ) );
  mTypeList.addType( mTypeVoidPtr    = new SvType("*void", CLASS_POINTER, 1, mTypeVoid ) );
  mTypeList.addType( mTypeFail       = new SvType("fail type", 0, 0, nullptr ) );
  mTypeList.addType( mTypeCBlock     = new SvType("cblock", CLASS_CBLOCK, 0, nullptr ) );
  mTypeList.addType( mTypeCBlockPtr  = new SvType("*cblock", CLASS_POINTER, 1, mTypeCBlock ) );
//  mTypeList.addType( mTypeStringPtr  = new SvType("*string", TTYPE_POINTER, 1, mTypeString ) );


  //Раздел коментариев
  mRemarkTable.clear();    //Таблица коментариев

  //Таблица ошибок
  mErrorList.clear();

  //mToken;                //Текущий токен
  //Распределение памяти
  mLocalAddress = 0;       //Очередной свободный адрес локальной переменной
  mLocalMax = 0;           //Максимальный адрес временных переменных внутри функции
  mGlobalAddress = 2;

  //Добавить переменную svTickCount;
  mVarGlobal.addVariable( new SvVariable(mark(), QString("svTickCount"), mTypeInt, 1, 0 ) );
  }


void
SvCompiler::InitKeyWords() {
  mKeyWords.clear();
  mKeyWords.insert( QString("_at_"),             tkwAt );
  mKeyWords.insert( QString("break"),            tkwBreak );
  mKeyWords.insert( QString("case"),             tkwCase );
  mKeyWords.insert( QString("bool"),             tkwBool );
  mKeyWords.insert( QString("catch"),            tkwCatch );
  mKeyWords.insert( QString("cblock"),           tkwCblock );
  mKeyWords.insert( QString("char"),             tkwChar );
  mKeyWords.insert( QString("class"),            tkwClass );
  mKeyWords.insert( QString("const"),            tkwConst );
  mKeyWords.insert( QString("const_cast"),       tkwConstCast );
  mKeyWords.insert( QString("continue"),         tkwContinue );
  mKeyWords.insert( QString("cstring"),          tkwCString );
  mKeyWords.insert( QString("default"),          tkwDefault );
  mKeyWords.insert( QString("delete"),           tkwDelete );
  mKeyWords.insert( QString("do"),               tkwDo );
  mKeyWords.insert( QString("double"),           tkwDouble );
  mKeyWords.insert( QString("dynamic_cast"),     tkwDynamicCast );
  mKeyWords.insert( QString("else"),             tkwElse );
  mKeyWords.insert( QString("enum"),             tkwEnum );
  mKeyWords.insert( QString("explicit"),         tkwExplicit );
  mKeyWords.insert( QString("export"),           tkwExport );
  mKeyWords.insert( QString("extern"),           tkwExtern );
  mKeyWords.insert( QString("false"),            tkwFalse );
  mKeyWords.insert( QString("float"),            tkwFloat );
  mKeyWords.insert( QString("for"),              tkwFor );
  mKeyWords.insert( QString("friend"),           tkwFriend );
  mKeyWords.insert( QString("function"),         tkwFunction );
  mKeyWords.insert( QString("goto"),             tkwGoto );
  mKeyWords.insert( QString("if"),               tkwIf );
  mKeyWords.insert( QString("import"),           tkwImport );
  mKeyWords.insert( QString("inline"),           tkwInline );
  mKeyWords.insert( QString("int"),              tkwInt );
  mKeyWords.insert( QString("long"),             tkwLong );
  mKeyWords.insert( QString("mutable"),          tkwMutable );
  mKeyWords.insert( QString("namespace"),        tkwNamespace );
  mKeyWords.insert( QString("new"),              tkwNew );
  mKeyWords.insert( QString("operator"),         tkwOperator );
  mKeyWords.insert( QString("private"),          tkwPrivate );
  mKeyWords.insert( QString("property"),         tkwProperty );
  mKeyWords.insert( QString("protected"),        tkwProtected );
  mKeyWords.insert( QString("public"),           tkwPublic );
  mKeyWords.insert( QString("register"),         tkwRegister );
  mKeyWords.insert( QString("reinterpret_cast"), tkwReinterpretCast );
  mKeyWords.insert( QString("return"),           tkwReturn );
  mKeyWords.insert( QString("short"),            tkwShort );
  mKeyWords.insert( QString("signed"),           tkwSigned );
  mKeyWords.insert( QString("sizeof"),           tkwSizeof );
  mKeyWords.insert( QString("static"),           tkwStatic );
  mKeyWords.insert( QString("static_cast"),      tkwStaticCast );
  mKeyWords.insert( QString("struct"),           tkwStruct );
  mKeyWords.insert( QString("switch"),           tkwSwitch );
  mKeyWords.insert( QString("template"),         tkwTemplate );
  mKeyWords.insert( QString("this"),             tkwThis );
  mKeyWords.insert( QString("throw"),            tkwThrow );
  mKeyWords.insert( QString("true"),             tkwTrue );
  mKeyWords.insert( QString("try"),              tkwTry );
  mKeyWords.insert( QString("typedef"),          tkwTypedef );
  mKeyWords.insert( QString("typeid"),           tkwTypeid );
  mKeyWords.insert( QString("typename"),         tkwTypename );
  mKeyWords.insert( QString("union"),            tkwUnion );
  mKeyWords.insert( QString("unsigned"),         tkwUnsigned );
  mKeyWords.insert( QString("using"),            tkwUsing );
  mKeyWords.insert( QString("vint"),             tkwVint );
  mKeyWords.insert( QString("virtual"),          tkwVirtual );
  mKeyWords.insert( QString("void"),             tkwVoid );
  mKeyWords.insert( QString("volatile"),         tkwVolatile );
  mKeyWords.insert( QString("wchar_t"),          tkwWchar_t );
  mKeyWords.insert( QString("while"),            tkwWhile );
  }






void
SvCompiler::ClearTables() {
  //Очистить таблицу типов
  mTypeList.clear();

  //Очистить таблицу глобальных переменных
  mVarGlobal.clear();

  //Очистить таблицу функций
  mFunGlobal.clear();

  //Очистить таблицу макроимен
  qDeleteAll(mMacroTable);
  mMacroTable.clear();           //Таблица макроимен

  //Очистить таблицу строк
  mStringTable.clear();

  //Таблица блоков данных
  mByteArrayTable.clear();
  }




int
SvCompiler::AllocLocal(SvType *type) {
  int addr = mLocalAddress;
  mLocalAddress += type->mSize;
  if( mLocalMax < mLocalAddress ) mLocalMax = mLocalAddress;
  return addr+1;
  }




int
SvCompiler::AllocGlobal(SvType *type) {
  int addr = mGlobalAddress;
  mGlobalAddress += type->mSize;
  return addr;
  }




QString SvCompiler::macroExpansion(const QString mac, const QString def)
  {
  if( mMacroTable.contains(mac) )
    return mMacroTable.value(mac)->mExpander;
  return def;
  }





/*
Функция "Сканирование. Точка входа в компиляцию"
Описание
  Открываем файл, далее пока не конец файла выполняем компиляцию
*/
void
SvCompiler::Compile( const QString &fname ) {
  DoFile( fname, true );
  NextToken();
  mFileId    = mToken.mFileId;
  mLineIndex = mToken.mLine;
  while( !mEof ) {
    if( mToken == tkwTypedef ) {
      Error( QObject::tr("Keyword typedef not supported.") );
      NextToken();
      }
    if( mToken == tkwExtern ) {
      Error( QObject::tr("Keyword extern not supported.") );
      NextToken();
      }
    else if( mToken == tkwStatic ) {
      Error( QObject::tr("Keyword static not supported.") );
      NextToken();
      }
    else if( mToken == tkwImport ) {
      NextToken();
      DoImport();
      }
    else if( mToken == tkwStruct ) {
      NextToken();
      DoStructure();
      continue;
      }

    DoDeclareGlobal();
    }
  }



