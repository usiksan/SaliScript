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

    Реализация класса TCompiler часть 9 (синтаксический разбор)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;






//Блок данных
void SvCompiler::DoCBlock()
  {
  //Убрать ключевое слово
  NextToken();
  //Должно быть имя
  if( mToken == ttName ) {
    //Проверить наличие переменной с таким именем
    if( !isGlobalSymbol(mToken.mString) ) {
      SvVariable *var = new SvVariable( mark(), mToken.mString, mTypeCBlock, mByteArrayTable.count() + 1, 0 );
      mVarGlobal.addVariable(var);
      }
    else Error( QObject::tr("Duplicate name %1").arg(mToken.mString) );
    NextToken();

    if( mToken != tsAssign )
      ErrorEndSt( QObject::tr("Error. Need =") );
    else {
      NextToken();

      if( mToken != tsFOpen )
        ErrorEndSt( QObject::tr("Error. Need {") );
      else {
        NextToken();
        QByteArray array;

        //Далее должны быть значения
        while( mToken != tsFClose ) {
          if( mToken != ttInt ) {
            ErrorEndSt( QObject::tr("Error. Need value") );
            break;
            }
          array.append( mToken.mIValue );
          NextToken();
          }

        if( mToken != tsFClose )
          ErrorEndSt( QObject::tr("Error. Need }") );
        else {
          NextToken();
          NeedSemicolon();
          }

        //Добавить полученный массив в таблицу
        mByteArrayTable.append( array );
        }
      }
    }
  else ErrorEndSt( QObject::tr("Error. Need const block name.") );
  }







void
SvCompiler::DoStructure() {
  SvStruct *type = nullptr;
  //Ожидается имя структуры
  if( mToken == ttName ) {
    if( isGlobalSymbol(mToken.mString) )
      Error( QObject::tr("Error. Duplicate class name \"%1\"").arg(mToken.mString) );
    type = mTypeList.addType( new SvStruct(mToken.mString) )->toStruct();
    NextToken();

    //Проверить наследование
    if( mToken == tsColon ) {
      //Эта структура наследует другую
      NextToken();
      //Должен быть тип базовой структуры
      if( mToken == ttType ) {
        //Базовая структура обнаружена
        if( mToken.mType->mClass != CLASS_STRUCT )
          Error( QObject::tr("Derivate must be from struct, but \"%1\" is not").arg( mToken.mString ) );
        else
          type->setBase( mToken.mType->toStruct() );
        NextToken();
        }
      else if( mToken == ttName ) {
        Error( QObject::tr("Undefined base class \"%1\"").arg(mToken.mString) );
        NextToken();
        }
      else {
        Error( QObject::tr("Need base class") );
        }
      }
    else if( mToken == ttType ) {
      if( mToken.mType->mClass == CLASS_STRUCT )
        type = mToken.mType->toStruct();
      else
        Error( QObject::tr("Type mismatch in struct \"%1\" redefinig").arg(mToken.mType->mName) );

      NextToken();
      }

    if( mToken == tsSemicolon ) {
      //Это было предварительное объявление
      NextToken();
      return;
      }

    //Должна следовать открывающая скобка
    if( mToken == tsFOpen ) {
      NextToken();

      //Только если структура не определена
      if( type->mDefined )
        Error( QObject::tr("Struct \"%1\" already defined early").arg(mToken.mType->mName) );
      else
        type->mDefined = true;

      //Объявить активную структуру
      mActiveStruct = type;

      //Сканировать члены структуры
      while( mToken != tsFClose && !mEof ) {
        //Записать тип переменной
        SvType *baseType = DoBaseType();
        //Должно быть имя типа
        if( baseType == 0 ) {
          ErrorEndSt( QObject::tr("Error. Need struct member type.") );
          NextToken();
          continue;
          }
        NextToken();
        while( !mEof ) {
          SvVariable *var = nullptr;
          SvFunction *fun = nullptr;
          DoSubType( baseType, var, fun, &(type->mSize) );
          if( var ) {
            if( var->mType->isCBlock() ) {
              Error( QObject::tr("Error. CBlock can not be member. Use cblock *ptr instead") );
              }
            //Определена переменная-член
            if( type->mMemberList.isPresent(var->mName) )
              Error( QObject::tr("Error. Member \"%1\" already defined early").arg(var->mName) );
            type->mMemberList.addVariable( var );
            }
          if( fun ) {
            //Определена функция-член
            Error( QObject::tr("Error. Member functions not allowed") );
            delete fun;
            }
          //Проверить следующее определение
          if( mToken != tsComma ) break;
          NextToken();
          }
        //Должен быть ;
        NeedSemicolon();
        }
      //Убрать закрывающую скобку
      NextToken();
      NeedSemicolon();
      mActiveStruct = nullptr;
      }
    else Error( QObject::tr("Error. Need struct definition.") );
    }
  else Error( QObject::tr("Error. Need struct name.") );
  }










void
SvCompiler::DoImport() {
  if( ConstExpression( &mActiveImport ) != mTypeInt )
    ErrorEndSt( QObject::tr("Error. Import index must be constant expression.") ); //Ожидается константное выражение
  }










SvType *SvCompiler::DoBaseType()
  {
  if( mToken == ttType )     return mToken.mType;
  if( mToken == tkwInt )     return mTypeInt;
  if( mToken == tkwVoid )    return mTypeVoid;
  if( mToken == tkwCString ) return mTypeCString;
  if( mToken == tkwCblock )  return mTypeCBlock;
  return nullptr;
  }




void SvCompiler::DoDeclareGlobal() {
  bool first = true;
  //Разобрать тип
  SvType *baseType = DoBaseType();
  if( baseType == mTypeCBlock ) DoCBlock();
  else if( baseType ) {
    NextToken();
    while( 1 ) {
      SvVariable *var = nullptr;
      SvFunction *fun = nullptr;
      DoSubType( baseType, var, fun, &mGlobalAddress );
      if( var == nullptr && fun == nullptr )
        return;
      //Разместить в таблицах
      if( var ) {
        if( mVarGlobal.isPresent( var->mName ) )
          Error( QObject::tr("Error. Duplicate variable name \"%1\".").arg( var->mName ) );
        mVarGlobal.addVariable( var );
        }
      if( fun ) {
        //Проверить наличие раннего объявления
        if( mFunGlobal.isPresent(fun->mName) ) {
          //Уже есть объявление или определение, проверим совпадение сигнатур
          if( mFunGlobal.getFunction(fun->mName)->mType != fun->mType )
            //Сигнатуры не совпадают
            Error( QObject::tr("Error. Function definition not equal previous.") );
          }

        if( mToken == tsSemicolon ) {
          //Это простое объявление функции, если нет раньше, то добавить
          if( !mFunGlobal.isPresent(fun->globalName()) ) {
            //Импортная функция
            fun->mImportIndex = mActiveImport;
            mActiveImport = 0;
            mFunGlobal.addFunction( fun, fun->globalName() );
            }
          else
            //Объявление уже есть, это просто удаляем
            delete fun;
          NextToken();
          return;
          }

        if( mToken == tsFOpen ) {
          //Идет тело функции
          if( !first )
            Error( QObject::tr("Error. Function definition can't be comma separated. It must be along.") );
          //Определение или объявление функции
          fun = mFunGlobal.addFunctionDefinition( fun );
          DoNewFunction( fun );
          return;
          }
        else {
          //Ожидаем тело функции
          Error( QObject::tr("Error. Need function body or semicolon.") );
          delete fun;
          return;
          }
        }
      if( mToken == tsFOpen ) {
        Error( QObject::tr("Error. Undefined context.") );
        return;
        }
      first = false;
      if( mToken == tsAssign ) {
        //Инициализация
        NextToken();
        }
      //Проверить допустимость типа
      if( var->mType->mClass == CLASS_VOID ) {
        //для void допустимы только указатели или функции
        Error( QObject::tr("Error. Variable can not be a void type.") );
        return;
        }
      if( mToken == tkwAt ) {
        NextToken();
        //Явное указание размещения
        int addr = 0;
        if( ConstExpression( &addr ) != mTypeInt ) {
          ErrorEndSt( QObject::tr("Error. Variable address must be constant expression.") ); //Ожидается константное выражение
          return;
          }
        var->mAddress = addr;
        }

      //Проверить следующее определение
      if( mToken != tsComma ) break;
      NextToken();
      }
    //Должен быть ;
    NeedSemicolon();
    }
  else ErrorEndSt( QObject::tr("Need type") );
  mActiveImport = 0;
  return;
  }








void
SvCompiler::DoParamList( SvFunction *fun ) {
  if( mToken != tsOpen ) {
    //Должна быть скобка
    ErrorEndSt( QObject::tr("Error. Need \'(\'.") );
    //Сформировать тип и распределить параметры
    DoParamAddress(fun);
    return;
    }
  NextToken();
  if( mToken != tsClose ) {
    while(1) {
      if( mEof ) {
        ErrorEndSt( QObject::tr("Error. Need closing bracket ')'.") );
        //Сформировать тип и распределить параметры
        DoParamAddress(fun);
        return;
        }
      SvType *type = DoBaseType();
      if( type == nullptr ) {
        ErrorEndSt( QObject::tr("Error. Need function argument type.") );
        //Сформировать тип и распределить параметры
        DoParamAddress(fun);
        return;
        }

      NextToken();
      //Сканировать параметр
      SvVariable *var = nullptr;
      SvFunction *subFun = nullptr;
      DoSubType( type, var, subFun, &(fun->mParamSize) );
      if( subFun ) {
        //Функции внутри списка параметров не допустимы
        ErrorEndSt( QObject::tr("Error. Function declaration not allowed in this context.") );
        delete subFun;
        //Сформировать тип и распределить параметры
        DoParamAddress(fun);
        return;
        }
      if( var ) {
        if( var->mType->isCBlock() ) {
          ErrorEndSt( QObject::tr("Error. CBlock can not be param, use cblock *ptr instead.") );
          }
        //Параметр считан нормально, добавить к списку параметров
        if( !fun->addParam( var ) ) {
          ErrorEndSt( QObject::tr("Error. Duplicate parametr name %1.").arg(var->mName) );
          //Сформировать тип и распределить параметры
          DoParamAddress(fun);
          return;
          }
        }
      else {
        //Тип ошибочный
        //Сформировать тип и распределить параметры
        DoParamAddress(fun);
        return;
        }
      if( mToken == tsComma ) {
        NextToken();
        continue;
        }
      if( mToken == tsClose ) break;
      ErrorEndSt( QObject::tr("Error. Need closing bracket ')'.") );
      //Сформировать тип и распределить параметры
      DoParamAddress(fun);
      return;
      }
    }
  //Готово, убрать скобку
  NextToken();

  //Сформировать тип и распределить параметры
  DoParamAddress(fun);
  }





void SvCompiler::DoParamAddress(SvFunction *fun)
  {
  //Сформировать тип и распределить параметры
  SvFunctionType *funType = new SvFunctionType();
  funType->mResult = fun->mResultType;

  //Назначить адреса параметрам
  for( SvVariable *var : fun->mParams.mList ) {
    var->mAddress = TFUN_FRAME_SIZE + fun->mParamSize - var->mAddress - var->mType->mSize;
    SvFunParam param(var);
    funType->mParamList.append( param );
    }

  fun->mType = mTypeList.addType( funType );
  }






void SvCompiler::DoSubType(SvType *type, SvVariablePtr &var, SvFunctionPtr &fun, int *addressCount)
  {
  QString varName;

  if( mToken == tsMul ) {
    //Указатель
    type = mTypeList.getTypePointer( type );
    NextToken();
    }

  if( mToken == tsOpen ) {
    //Открывающая скобка, подразумевается указатель на функцию
    NextToken();
    if( mToken == tsMul ) {
      //Указатель на функцию
      NextToken();
      }
    if( mToken != ttName ) {
      ErrorEndSt( QObject::tr("Error. Need name.") ); //Ожидается имя
      return;
      }
    varName = mToken.mString;
    SvMark nameMark = mark();
    NextToken();
    if( mToken != tsClose ) {
      ErrorEndSt( QObject::tr("Error. Need closed bracket ')'.") );
      return;
      }
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( QObject::tr("Error. Invalid definition of function pointer variable.") );
      return;
      }
    SvFunction *func = new SvFunction( varName, type, nameMark );
    DoParamList( func );
    //Из этой функции нам нужен только тип
    SvType *funPtrType = mTypeList.getTypePointer( func->mType );
    delete func;
    var = new SvVariable( nameMark, varName, funPtrType, *addressCount );
    }

  else if( mToken == ttName ) {
    varName = mToken.mString;
    SvMark nameMark = mark();
    NextToken();
    if( mToken == tsOpen ) { //Определение функций допустимо только в глобальном контексте
      if( mActiveFunction )
        ErrorEndSt( QObject::tr("Error. Function definition allowed only in global context.") );
      //Это функция
      fun = new SvFunction( varName, type, nameMark );
      DoParamList( fun );
      //Дальше будет разбираться функция
      return;
      }
    //Простая переменная, сформировать
    var = new SvVariable( nameMark, varName, type, *addressCount );
    }

  else {
    //Неизвестный контекст
    ErrorEndSt( QObject::tr("Error. Undefined context.") );
    return;
    }

  //Проверить массив
  if( mToken == tsSOpen ) {
    NextToken();
    //Проверить допустимость элементов массива
    if( type->mClass == CLASS_VOID )
      Error( QObject::tr("Error. Array of void types not allowed.") );

    //Должно быть константное выражение размерности массива
    if( mToken == tsSClose ) {
      NextToken();
      //Открытый массив не допустим
      ErrorEndSt( QObject::tr("Error. Open array is not allowed.") );
      return;
      }

    //Фиксированный массив
    int count = 0;
    if( ConstExpression( &count ) != mTypeInt ) {
      ErrorEndSt( QObject::tr("Error. Array dimension must be constant int expression.") ); //Ожидается константное выражение
      return;
      }
    if( count > 65535 )
      Error( QObject::tr("Error. Too big array! Only 65535 elements max.") );
    if( count == 0 )
      Error( QObject::tr("Error. Zero elements not allowed.") );
    var->mType = type = mTypeList.getTypeArray( type, count );
    var->mArraySize = count;
    //Должен быть ]
    if( mToken != tsSClose ) {
      ErrorEndSt( QObject::tr("Error. Need closing bracket ']'") );
      return;
      }
    NextToken();
    }

  (*addressCount) += var->getSize();
  }





void
SvCompiler::DoNewFunction( SvFunction *fun ) {
  if( fun == 0 ) {
    //Идет тело не для функции
    Error( QObject::tr("Error. Function body is not allowed in this context.") );
    return;
    }
  if( mToken == tsSemicolon ) {
    //Это предварительное определение
    NextToken();
    return;
    }
  //Все параметры уже считаны, а сама функция находится в fun
  //Это новое определение
  if( fun->mDefined )
    //Тело уже определено
    Error( QObject::tr("Error. Function body %1 already defined earlier.").arg(fun->mName) );


  //Идет тело функции
  fun->mMarkDefine = mark();
  fun->setRemark( mPrevRemark, mAfterRemark );
  mActiveFunction = fun;
  mActiveFunction->mDefined = true;
  if( mToken == tsFOpen ) {
    //Инициализироваться для разбора тела функции
    //Установить смещения временных переменных
    mLocalMax = mLocalAddress = 0;
    //Блок тела функции
    fun->mBody = DoCompound();
    if( mLastStatement != tstReturn ) {
      SvOperator *ret = new SvOperatorReturn( mark(), QString(), mActiveFunction );
      fun->mBody->mList.append( ret );
      }
    //Эпилог функции
    mActiveFunction->mLocalAmount = mLocalMax;
    }
  else ErrorEndSt( QObject::tr("Error. Need function body.") );
  mActiveFunction = 0;
  mActiveStruct = 0;
  }








SvOperatorBlock *SvCompiler::DoCompound() {
  NextToken();
  SvOperatorContext *savContext = mContext; //Сохранить предыдущий контекст
  SvOperatorBlock *block = new SvOperatorBlock( mark(), QString(), mContext );
  mContext = block;
  int savTempOffset = mLocalAddress; //Сохранить положение стека временных переменных
  while( mToken != tsFClose ) {
    if( mEof ) {
      Error( QObject::tr("Error. Need closing bracket }.") );
      break;
      }
    SvOperator *op = DoStatement( block );
    if( op )
      block->mList.append( op );
    }
  //Восстановить положение стека временных переменных
  mLocalAddress = savTempOffset;
  //Восстановить предыдущий контекст
  mContext = savContext;
  //Пометить место завершения блока
  block->mEnd = mark();
  //Убрать }
  NextToken();
  return block;
  }
















SvOperator *SvCompiler::DoStatement( SvOperator *parent ) {
  SvOperator *op = nullptr;
  switch( mToken ) {
    case tkwStatic   :
    case tkwConst    :
    case tkwVoid     :
    case tkwFloat    :
    case tkwDouble   :
    case tkwBool     :
    case tkwUnsigned :
    case tkwSigned   :
    case tkwChar     :
    case tkwLong     :
    case tkwShort    :
    case tkwInt      :
    case ttType      : op = DoLocal();    mLastStatement = tstLocal; break;
    case tsFOpen     : op = DoCompound(); break;
    case tkwIf       : op = DoIf();       mLastStatement = tstIf; break;
    case tkwWhile    : op = DoWhile();    mLastStatement = tstWhile; break;
    case tkwDo       : op = DoDo();       mLastStatement = tstDo; break;
    case tkwFor      : op = DoFor();      mLastStatement = tstFor; break;
    case tkwSwitch   : op = DoSwitch();   mLastStatement = tstSwitch; break;
    case tkwCase     : op = DoCase();     mLastStatement = tstCase; break;
    case tkwDefault  : op = DoDefault();  mLastStatement = tstDefault; break;
    case tkwReturn   : op = DoReturn();   mLastStatement = tstReturn; break;
    case tkwBreak    : op = DoBreak();    mLastStatement = tstBreak; break;
    case tkwContinue : op = DoContinue(); mLastStatement = tstContinue; break;
    case tsSemicolon : NextToken();  break;
    default          : op = DoExpression(); mLastStatement = tstExpression; break;
    }
  if( op ) op->mParent = parent;
  return op;
  }





SvOperator *SvCompiler::DoLocal() {   //Оператор определения локальных переменных
  //Разобрать тип
  SvType *baseType = DoBaseType();
  NextToken();
  if( baseType == 0 ) {
    Error( QObject::tr("Error. Not allowed type.") );
    return 0;
    }
  //Определить блок для инициализации переменных
  SvOperatorBlock *initBlock = new SvOperatorBlock( mark(), QString(), mContext );
  while( 1 ) {
    SvVariable *var = nullptr;
    SvFunction *fun = nullptr;
    DoSubType( baseType, var, fun, &mLocalAddress );
    //Скорректировать максимальный размер
    if( mLocalMax < mLocalAddress )
      mLocalMax = mLocalAddress;

    if( fun ) {
      //Внутри функции определена еще одна функция
      Error( QObject::tr("Error. Function not allowed inside other function.") );
      delete fun;
      }

    if( var == nullptr ) return initBlock;

    //Проверить такое-же имя в текущем контексте
    if( mContext->mVarLocal.isPresent(var->mName) )
      Error( QObject::tr("Error. Duplicate name %1.").arg(var->mName) );

    if( var->mType->isCBlock() ) {
      ErrorEndSt( QObject::tr("Error. CBlock can not be local variable, use cblock *ptr instead.") );
      }

    //Добавить переменную
    mContext->mVarLocal.addVariable( var );

    //Проверить инициализацию
    if( mToken == tsAssign ) {
      NextToken();
      SvOperatorExpression *expr = new SvOperatorExpression( mark(), curLine() );
      SvValueStore *store = new SvValueStore( new SvValueVariable( var, mark() ), mark() );
      //Вычислить выражение
      B0(store->mOperand2);
      expr->mExpression = store;
      var->mInit = store;
      initBlock->mList.append( expr );
      }
    //Проверить следующее определение
    if( mToken != tsComma ) break;
    NextToken();
    }
  //Должен быть ;
  NeedSemicolon();
  return initBlock;
  }





SvOperator *SvCompiler::DoIf() {  //Оператор if
  NextToken(); //Убрать if
  if( mToken != tsOpen ) Error( Need('(') );
  else NextToken(); //Убрать скобку
  SvOperatorIf *svIf = new SvOperatorIf( mark(), curLine() );
  //Условие
  BExpression( svIf->mCondition );
  if( mToken != tsClose ) Error( Need(')') );
  else NextToken(); //Убрать скобку
  svIf->mTrue = DoStatement( svIf );
  if( mToken == tkwElse ) {
    NextToken();
    svIf->mFalse = DoStatement( svIf );
    }
  return svIf;
  }





SvOperator *SvCompiler::DoWhile() {  //Оператор while
  NextToken(); //Убрать while
  if( mToken != tsOpen ) Error( Need('(') );
  else NextToken(); //Убрать скобку
  SvOperatorWhile *svWhile = new SvOperatorWhile( mark(), curLine(), mContext );

  //Сохранить предыдущий цикл
  SvOperatorLoop *loop = mLoop;
  mLoop = svWhile;

  //Начало цикла
  //Вычисление условия завершения
  //Условие цикла
  BExpression( svWhile->mCondition );
  if( mToken != tsClose ) Error( Need(')') );
  else NextToken(); //Убрать скобку
  svWhile->mBody = DoStatement( svWhile );

  //Восстановить предыдущий цикл
  mLoop = loop;

  return svWhile;
  }





SvOperator *SvCompiler::DoFor() {  //Оператор for
  NextToken(); //Убрать for
  if( mToken != tsOpen ) Error( QObject::tr("Error. Need \'(\'.") );
  else NextToken(); //Убрать скобку
  SvOperatorFor *svFor = new SvOperatorFor( mark(), curLine(), mContext );

  //Сохранить предыдущий цикл
  SvOperatorLoop *loop = mLoop;
  mLoop = svFor;

  //Вычисление установки начальных значений
  if( mToken != tsSemicolon ) {
    //Пытался сделать возможность определения переменных прямо в цикле
    // плюнул из-за сложности
    //    SvType *type = DoBaseType();
    //    if( type == nullptr )
    BExpression( svFor->mInit );
    }
  NeedSemicolon();

  //Вычисление условия
  if( mToken != tsSemicolon ) {
    BExpression( svFor->mCondition );
    }
  NeedSemicolon();

  //Вычисление постциклового выражения
  if( mToken != tsClose ) {
    BExpression( svFor->mAction );
    }
  if( mToken != tsClose ) Error( QObject::tr("Error. Need closing bracket ')'.") );
  else NextToken(); //Убрать скобку

  //Оператор
  svFor->mBody = DoStatement( svFor );

  //Восстановить предыдущий цикл
  mLoop = loop;

  return svFor;
  }





SvOperator *SvCompiler::DoDo() {  //Оператор do
  NextToken(); //Убрать do
  SvOperatorDoWhile *svDoWhile = new SvOperatorDoWhile( mark(), curLine(), mContext );

  //Сохранить предыдущий цикл
  SvOperatorLoop *loop = mLoop;
  mLoop = svDoWhile;

  //Начало цикла
  svDoWhile->mBody = DoStatement( svDoWhile );
  if( mToken != tkwWhile ) Error( Need("while") );
  else NextToken(); //Убрать while

  if( mToken != tsOpen ) Error( Need('(') );
  else NextToken(); //Убрать скобку

  //Вычисление условия завершения
  //Условие цикла
  BExpression( svDoWhile->mCondition );
  if( mToken != tsClose ) Error( QObject::tr("Error. Need closing bracket ')'.") );
  else NextToken(); //Убрать скобку

  NeedSemicolon();

  //Восстановить предыдущий цикл
  mLoop = loop;

  return svDoWhile;
  }





SvOperator *SvCompiler::DoSwitch() {  //Оператор switch
  ErrorEndSt( QObject::tr("Error. switch not supported.") );
  return nullptr;
  }





SvOperator *SvCompiler::DoCase() {   //Оператор case
  ErrorEndSt( QObject::tr("Error. switch not supported.") );
  return nullptr;
  }





SvOperator *SvCompiler::DoDefault() { //Оператор default
  ErrorEndSt( QObject::tr("Error. switch not supported.") );
  return nullptr;
  }





SvOperator *SvCompiler::DoReturn() {  //Оператор return
  SvOperatorReturn *svReturn = new SvOperatorReturn( mark(), curLine(), mActiveFunction );
  NextToken(); //Убрать return
  if( mActiveFunction->mType->toFunction()->mResult->mClass != CLASS_VOID ) {
    //Возвращает какое-то значение
    if( mToken == tsSemicolon )
      Error( QObject::tr("Error. Need expression type %1.").arg( mActiveFunction->mType->toFunction()->mResult->mName ) );
    else {
      BExpression( svReturn->mResult );
      }
    }
  NeedSemicolon();
  //Возврат
  return svReturn;
  }





SvOperator *SvCompiler::DoBreak() { //Оператор break
  SvOperator *op = new SvOperatorBreak( mark(), curLine(), mLoop );
  NextToken(); //Убрать break
  if( mLoop == nullptr )
    Error( QObject::tr("Error. break outside loop or switch operator.") ); //break вне цикла или switch
  NeedSemicolon();
  return op;
  }





SvOperator *SvCompiler::DoContinue() { //Оператор continue
  SvOperator *op = new SvOperatorContinue( mark(), curLine(), mLoop );
  NextToken(); //Убрать contionue
  if( mLoop == nullptr )
    Error( QObject::tr("Error. continue outside loop.") ); //continue вне цикла
  NeedSemicolon();
  return op;
  }





SvOperator *SvCompiler::DoExpression() { //Оператор выражение
  SvOperatorExpression *expr = new SvOperatorExpression( mark(), curLine() );
  BExpression( expr->mExpression );
  NeedSemicolon();
  return expr;
  }





