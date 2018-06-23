/*
Проект "Компилятор"
Содержание
  Реализация класса TCompiler часть 1 (конструктор и инициализация)
  Конструктор
    TCompiler();
  Функции:
    void Reset(); //Инициализация
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

/*
Конструктор по умолчанию
*/
SrCompiler::SrCompiler() :
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

SrCompiler::~SrCompiler() {
  ClearTables();
  }

void
SrCompiler::Reset() {
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
  mMacroTable.clear();     //Таблица макроимен
  mMacroTable.insert( QString("VPU_COMPILER"), new SrMacro() );
  mStringTable.clear();    //Очистить таблицу строк
  mStringTable.append( QString() ); //По индексу 0 добавить пустую строку


  mLastStatement = 0;            //Идентификатор последнего обработанного оператора
  mActiveFunction = nullptr;     //Обрабатываемая функция
  mActiveStruct = nullptr;       //Обрабатываемая структура
  mActiveImport = 0;             //Обрабатывается import
  mContext = nullptr;            //Текущий контекст
  mLoop = nullptr;               //Активный оператор цикла


  //Раздел типов
  ClearTables();           //Указатели на все типы
  //Создать типы по умолчанию
  mTypeList.addType( mTypeInt        = new SrType("int", TTYPE_INT, 1, 0) );  //Целое
  mTypeList.addType( mTypeIntPtr     = new SrType("*int", TTYPE_POINTER, 1, mTypeInt ) );
  mTypeList.addType( mTypeCString    = new SrType("cstring", TTYPE_CSTRING, 1, 0 ) );
  mTypeList.addType( mTypeCStringPtr = new SrType("*cstring", TTYPE_POINTER, 1, mTypeCString ) );
  mTypeList.addType( mTypeVoid       = new SrType("void", TTYPE_VOID, 0, 0 ) );
  mTypeList.addType( mTypeVoidPtr    = new SrType("*void", TTYPE_POINTER, 1, mTypeVoid ) );
  mTypeList.addType( mTypeFail       = new SrType("fail type", 0, 0, 0 ) );
  mTypeList.addType( mTypeCBlock     = new SrType("cblock", TTYPE_CBLOCK, 0, 0 ) );
  mTypeList.addType( mTypeCBlockPtr  = new SrType("*cblock", TTYPE_POINTER, 1, mTypeCBlock ) );
//  mTypeList.addType( mTypeStringPtr  = new SrType("*string", TTYPE_POINTER, 1, mTypeString ) );


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
  mVarGlobal.addVariable( new SrVariable(mark(), QString("svTickCount"), mTypeInt, 1, 0 ) );

  //Очистить список сцен и инициализировать его сценами по умолчанию
  ClearSceneList();
  }



void
SrCompiler::ClearTables() {
  //Очистить таблицу типов
  mTypeList.clear();

  //Очистить таблицу глобальных переменных
  mVarGlobal.clear();

  //Очистить таблицу функций
  mFunGlobal.clear();
  }




int
SrCompiler::AllocLocal(SrType *type) {
  int addr = mLocalAddress;
  mLocalAddress += type->mSize;
  if( mLocalMax < mLocalAddress ) mLocalMax = mLocalAddress;
  return addr+1;
  }




int
SrCompiler::AllocGlobal(SrType *type) {
  int addr = mGlobalAddress;
  mGlobalAddress += type->mSize;
  return addr;
  }
