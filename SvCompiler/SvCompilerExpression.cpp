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

    Реализация класса TCompiler часть 8 (разбор выражений)
*/
#include "SvCompiler.h"

using namespace SvCompiler6;



SvType* SvCompiler::ConstExpression(int *intRes) {
  //Сохранить текущий указатель команды
  SvValuePtr val = nullptr;
  B2( val );
  if( val ) {
    SvType *res = mTypeFail;
    constValueCalc( val );
    if( val->isConst() ) {
      res = val->getType();
      *intRes = val->mConstInt;
      }
    delete val;
    return res;
    }
  return mTypeFail;
  }









void
SvCompiler::BExpression( SvValuePtr &val ) {
  B0( val );
  if( mToken == tsComma ) {
    SvValueComma *comma = new SvValueComma( val, mark() );
    //Операция ','
    while(1) {
      if( mToken != tsComma ) break;
      //Проверить доступность операндов
      if( !comma->isOperandAvailable() )
        Error( QObject::tr("Error. Too many values in comma operation. Max %1").arg(SV_OPER_MAX) );
      //Вычислить значение
      B0( comma->nextOperand() );
      NextToken();
      }
    }
  }












/*
B0 операция присваивания
*/
void
SvCompiler::B0( SvValuePtr &res ) {
  //Операция присваивания
  B1( res ); //Получить левую часть
  SvValueBinary *oper = nullptr;
  switch( mToken ) {
    case tsAssign    : oper = new SvValueStore( res, mark() );       break; // =
    case tsPlusAsg   : oper = new SvValueAddStore( res, mark() );    break; // +=
    case tsMinusAsg  : oper = new SvValueSubStore( res, mark() );    break; // -=
    case tsMulAsg    : oper = new SvValueMulStore( res, mark() );    break; // *=
    case tsDivAsg    : oper = new SvValueDivStore( res, mark() );    break; // /=
    case tsModAsg    : oper = new SvValueModStore( res, mark() );    break; // %=
    case tsXorAsg    : oper = new SvValueXorStore( res, mark() );    break; // ^=
    case tsAndAsg    : oper = new SvValueAndStore( res, mark() );    break; // &=
    case tsOrAsg     : oper = new SvValueOrStore( res, mark() );     break; // |=
    case tsLShiftAsg : oper = new SvValueLShiftStore( res, mark() ); break; // <<=
    case tsRShiftAsg : oper = new SvValueRShiftStore( res, mark() ); break; // >>=
    default : return;
    }
  res = oper;
  NextToken();

  //Получить правую часть
  B0( oper->mOperand2 );
  }









/*
B1 операция ? :
*/
void
SvCompiler::B1( SvValuePtr &val ) {
  B2( val );
  if( mToken == tsQuestion ) {
    SvValueCondition *cond = new SvValueCondition( val, mTypeInt, mark() );
    val = cond;
    NextToken(); //Убрать ?
    B2(cond->mTrue);

    if( mToken != tsColon ) {
      ErrorEndSt( QObject::tr("Need :") );
      return;
      }

    NextToken(); //Убрать :
    B2(cond->mFalse);
    }
  }









/*
B2 операция ||
*/
void
SvCompiler::B2( SvValuePtr &val ) {
  B3( val );
  if( mToken == tsLOr ) {
    SvValueBinaryLong *orr = new SvValueLogOr( val, mTypeInt, mark() );
    val = orr;

    while( mToken == tsLOr ) {
      NextToken();
      //Проверить доступность операндов
      if( !orr->isOperandAvailable() )
        Error( QObject::tr("Error. Too mani values in 'or' operation. Max %1").arg(SV_OPER_MAX) );
      B3( orr->nextOperand() );
      }

    }
  }









/*
B3 операция &&
*/
void
SvCompiler::B3( SvValuePtr &val ) {
  B4( val );
  if( mToken == tsLAnd ) {
    SvValueBinaryLong *andd = new SvValueLogAnd( val, mTypeInt, mark() );
    val = andd;

    while( mToken == tsLAnd ) {
      NextToken();
      //Проверить доступность операндов
      if( !andd->isOperandAvailable() )
        Error( QObject::tr("Error. To mani values in 'and' operation. Max %1").arg(SV_OPER_MAX) );
      B4( andd->nextOperand() );
      }
    }
  }









/*^
B4 операция |
*/
void
SvCompiler::B4( SvValuePtr &val ) {
  B5( val );
  if( mToken == tsOr ) {
    SvValueBinaryLong *bin = new SvValueOr( val, mTypeInt, mark() );
    val = bin;
    while( mToken == tsOr ) {
      NextToken();
      //Проверить доступность операндов
      if( !bin->isOperandAvailable() )
        Error( QObject::tr("Error. To mani values in 'binary or' operation. Max %1").arg(SV_OPER_MAX) );
      B5( bin->nextOperand() );
      }
    }
  }









/*^
B5 операция ^
*/
void
SvCompiler::B5(SvValuePtr &val ) {
  B6( val );

  if( mToken == tsXor ) {
    SvValueBinaryLong *bin = new SvValueXor( val, mTypeInt, mark() );
    val = bin;
    while( mToken == tsXor ) {
      NextToken();
      //Проверить доступность операндов
      if( !bin->isOperandAvailable() )
        Error( QObject::tr("Error. To mani values in 'xor' operation. Max %1").arg(SV_OPER_MAX) );
      B6( bin->nextOperand() );
      }
    }
  }









/*^
B6 операция &
*/
void
SvCompiler::B6( SvValuePtr &val ) {
  B7( val );
  if( mToken == tsAnd ) {
    SvValueBinaryLong *bin = new SvValueAnd( val, mTypeInt, mark() );
    val = bin;
    while( mToken == tsAnd ) {
      NextToken();
      //Проверить доступность операндов
      if( !bin->isOperandAvailable() )
        Error( QObject::tr("Error. To mani values in 'binary and' operation. Max %1").arg(SV_OPER_MAX) );
      B7( bin->nextOperand() );
      }
    }
  }









/*
B7 операция == и !=
*/
void
SvCompiler::B7(SvValuePtr &val ) {
  B8( val );
  while( 1 ) {
    SvValueBinary *oper = nullptr;
    if( mToken == tsEqu )         oper = new SvValueEqu( val, mTypeInt, mark() );
    else if( mToken == tsNotEqu ) oper = new SvValueNotEqu( val, mTypeInt, mark() );
    else break;
    val = oper;
    NextToken();
    B8( oper->mOperand2 );
    }
  }









/*
B8 операция <=  >=  <  >
*/
void
SvCompiler::B8(SvValuePtr &val ) {
  B9( val );
  while( 1 ) {
    SvValueBinary *oper = nullptr;
    switch( mToken ) {
      case tsLessEqu : oper = new SvValueLessEqu( val, mTypeInt, mark() ); break;
      case tsGrowEqu : oper = new SvValueGreatEqu( val, mTypeInt, mark() ); break;
      case tsLess    : oper = new SvValueLess( val, mTypeInt, mark() ); break;
      case tsGrow    : oper = new SvValueGreat( val, mTypeInt, mark() ); break;
      default : return;
      }
    val = oper;
    NextToken();
    B9( oper->mOperand2 );
    }
  }









/*^
B9 операция << и >>
*/
void
SvCompiler::B9(SvValuePtr &val ) {
  B10( val );
  while( 1 ) {
    SvValueBinary *oper = nullptr;
    if( mToken == tsLShift )      oper = new SvValueLShift( val, mark() );
    else if( mToken == tsRShift ) oper = new SvValueRShift( val, mark() );
    else break;
    val = oper;
    NextToken();
    B10( oper->mOperand2 );
    }
  }









/*^
B10 операция + и -
*/
void
SvCompiler::B10(SvValuePtr &val ) {
  B11( val );
  while( 1 ) {
    SvValueBinary *oper = nullptr;
    if( mToken == tsPlus )       oper = new SvValueAdd( val, mark() );
    else if( mToken == tsMinus ) oper = new SvValueSub( val, mark() );
    else break;
    val = oper;
    NextToken();
    B11( oper->mOperand2 );
    }
  }









/*
B11 операция *  /  %
*/
void
SvCompiler::B11( SvValuePtr &val ) {
  B12( val );
  while( 1 ) {
    SvValueBinary *oper = nullptr;
    if( mToken == tsMul )      oper = new SvValueMul( val, mark() );
    else if( mToken == tsDiv ) oper = new SvValueDiv( val, mark() );
    else if( mToken == tsMod ) oper = new SvValueMod( val, mark() );
    else break;
    val = oper;
    NextToken();
    B12( oper->mOperand2 );
    }
  }









/*
B12 операция ++val --val ~val !val -val +val *val &val val++ val--
*/
void
SvCompiler::B12( SvValuePtr &val ) {
  switch( mToken ) {
    case tsInc :   // ++
      NextToken();
      B12( val );
      val = new SvValuePredInc( val, mark() );
      break;
    case tsDec :   // --
      NextToken();
      B12( val );
      val = new SvValuePredDec( val, mark() );
      break;
    case tsNot :   // ~
      NextToken();
      B12( val );
      val = new SvValueBitNot( val, mark() );
      break;
    case tsLNot :  // !
      NextToken();
      B12( val );
      val = new SvValueLogNot( val, mTypeInt, mark() );
      break;
    case tsPlus :  // +
      NextToken();
      B12( val );
      break;
    case tsMinus : // -
      NextToken();
      B12( val );
      val = new SvValueNeg( val, mark() );
      break;
    case tsMul :   // *
      NextToken();
      B12( val );
      val = new SvValuePointer( val, mark() );
      break;
    case tsAnd :   // &
      NextToken();
      B12( val );
      val = new SvValueAddress( val, mark() );
      break;
    default :
      B13( val );
      if( mToken == tsInc ) {
        val = new SvValuePostInc( val, mark() );
        }
      else if( mToken == tsDec ) {
        val = new SvValuePostDec( val, mark() );
        }
      else break;
      NextToken();
    }
  }









/*
B13 операция [val] val() val. val->
*/
void
SvCompiler::B13(SvValuePtr &val ) {
  B15( val );
  while( 1 ) {

    if( mToken == tsSOpen ) {
      //Обработка массивов [val]
      SvValueArrayCell *array = new SvValueArrayCell( val, mark() );
      val = array;
      // [val] - массив
      NextToken();
      B0( array->mOperand2 );
      if( mToken != tsSClose ) Error( Need(']') );
      else NextToken();
      }

    else if( mToken == tsOpen ) {
      //Вызов функции ( <val <,val>... > )
      NextToken();
      SvFunctionType *fun = nullptr;
      if( val->getClass() & CLASS_FUNCTION  )
        fun = val->getType()->toFunction();
      else if( (val->getClass() & CLASS_POINTER) && (val->getType()->mBaseType->mClass & CLASS_FUNCTION) )
        fun = val->getType()->mBaseType->toFunction();

      if( fun != nullptr ) {
        //Инициализировать загрузку параметров
        //Вызов
        SvValueCall *call = new SvValueCall( val, fun->mResult, mark() );
        val = call;
        int i = 0;

        if( mToken != tsClose ) {
          //Сканировать все параметры
          while( true ) {
            if( mEof ) {
              Error( Need(')') );
              return;
              }
            //Идет очередной параметр
            if( !call->isParamAvailable() )
              Error( QObject::tr("Error. Too many params.") );
            B0( call->nextParam() );
            i++;
            if( mToken != tsComma ) break;
            NextToken();
            }
          }

        //Должна следовать закрывающая скобка
        if( mToken == tsClose ) {
          NextToken();
          //Проверить количество параметров
          if( i != fun->mParamList.count() )
            Error( QObject::tr("Error. Invalid parametrs number in function call.") );
          }
        else Error( Need(')') );
        }
      else {
        Error( QObject::tr("Error. Can not function call there.") );
        val = new SvValueConstInt( 0, mTypeInt, mark() );
        }
      }
    else if( mToken == tsPoint ) {
      //Достать член структуры
      NextToken();
      //Операция доставания
      B14Member( val );
      }
    else if( mToken == tsRef ) {
      if( val->getType() && val->getClass() == CLASS_POINTER && val->getType()->mBaseType->mClass & CLASS_STRUCT ) {
        val = new SvValuePointer( val, mark() );
        NextToken();
        //Операция доставания
        B14Member( val );
        }
      else ErrorEndSt( QObject::tr("Error. Member reference to nonstruct.") );
      }
    else break;
    }
  }










/*
B14 операция доставания члена
*/
void SvCompiler::B14Member(SvValuePtr &val)
  {
  if( val->getClass() & CLASS_STRUCT ) {
    SvStruct *cls = val->getType()->toStruct();
    if( mToken == ttName ) {
      //Получено имя, проверим, является ли оно членом данной структуры
      SvVariable *var = cls->getMember(mToken.mString);
      if( var ) val = new SvValueMemberVariable( var, val, mark() );
      else ErrorEndSt( QObject::tr("Error. \"%1\" not member of struct \"%2\"").arg(mToken.mString).arg( cls->mName ) );
      NextToken();
      }
    else ErrorEndSt( QObject::tr("Error. Need struct member.") );
    }
  else ErrorEndSt( QObject::tr("Error. Member reference to nonstruct.") );
  }






/*
B15 операция (val) val()
*/
void
SvCompiler::B15( SvValuePtr &val ) {
  switch( mToken ) {
    case tsOpen :
      //Открывающая скобка - выражение
      //Убрать скобку
      NextToken();
      //Изменение приоритета скобками ( выражение )
      //Сохранить переходы
      BExpression( val );
      if( mToken != tsClose ) Error( Need(')') );
      break;
    case ttName : {
      //Проверить специальную функцию Wait
      if( BoWaitFunction(val) || BoCatchFunction(val) || BoThrowFunction(val) || BoExceptionFunction(val) )
        break;
      SvVariable *var = nullptr;
      SvFunction *fun = nullptr;
      //Если не найдено, то ищем по иерархии блоков
      if( mContext )
        var = mContext->getLocal( mToken.mString );

      //Если ничего не найдено и активна функция, то ищем по ее параметрам
      if( var == nullptr && fun == nullptr && mActiveFunction )
        var = mActiveFunction->getParam( mToken.mString );

      //Если ничего не найдено, то ищем по глобальным спискам
      if( var == nullptr && fun == nullptr ) {
        var = mVarGlobal.mHash.value( mToken.mString, nullptr );
        fun = mFunGlobal.mHash.value( mToken.mString, nullptr );
        }

      //Если и теперь ничего найдено, то фиксируем ошибку
      if( var == nullptr && fun == nullptr ) {
        //Вообще ничего не найдено
        //Выдать ошибку
        Error( QObject::tr("Error. Undefined identifier \"%1\".").arg(mToken.mString) ); // Неопределенный идентификатор
        //Заполнить val константой
        val = new SvValueConstInt( 0, mTypeInt, mark() );
        }
      else {
        if( var ) val = new SvValueVariable( var, mark() );
        else      val = new SvValueFunction( fun, mark() );
        }
      }
      break;
    case ttInt :
      //val - целая константа
      val = new SvValueConstInt( mToken.mIValue, mTypeInt, mark() );
      break;
    case ttFloat :
      //val - константа с плавающей точкой
      errorInLine( QObject::tr("Error. Float unsupported"), mark() );
      val = new SvValueConstInt( 0, mTypeInt, mark() );
      break;
    case ttString :
      //val - индекс строки в таблице строк
      val = new SvValueConstString( mStringTable.count(), mTypeCString, mark() );
      mStringTable.append( mToken.mString );
      break;
    default :
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      ErrorEndSt( QObject::tr("Error. Undefined context.") );
      return;
    }
  NextToken();
  }







bool
SvCompiler::BoWaitFunction( SvValuePtr &val) {
  if( mToken.mString == QString("svWait") ) {
    //Доразобрать
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( Need('(') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    NextToken();
    if( mToken != tsClose ) {
      ErrorEndSt( Need(')') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    //Генерировать
    val = new SvValueWaitFun( mTypeVoid, mark() );
    return true;
    }
  return false;
  }





bool SvCompiler::BoCatchFunction(SvValuePtr &val)
  {
  if( mToken.mString == QString("svCatch") ) {
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( Need('(') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    NextToken();
    SvValuePtr catchValue = nullptr;
    B0( catchValue );
    if( catchValue == nullptr ) {
      ErrorEndSt( QObject::tr("Need catch mask") );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    if( mToken != tsClose ) {
      ErrorEndSt( Need(')') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    //Генерировать
    val = new SvValueCatchFun( catchValue, mTypeVoid, mark() );
    return true;
    }
  return false;
  }








bool SvCompiler::BoThrowFunction(SvValuePtr &val)
  {
  if( mToken.mString == QString("svThrow") ) {
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( Need('(') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    NextToken();
    SvValuePtr throwValue = nullptr;
    B0( throwValue );
    if( throwValue == nullptr ) {
      ErrorEndSt( QObject::tr("Need throw signal value") );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    if( mToken != tsClose ) {
      ErrorEndSt( Need(')') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    //Генерировать
    val = new SvValueThrowFun( throwValue, mTypeVoid, mark() );
    return true;
    }
  return false;
  }









bool SvCompiler::BoExceptionFunction(SvValuePtr &val)
  {
  if( mToken.mString == QString("svException") ) {
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( Need('(') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    NextToken();
    if( mToken != tsClose ) {
      ErrorEndSt( Need(')') );
      //Установить заглушку
      val = new SvValueError( mTypeVoid, mark() );
      return true;
      }
    //Генерировать
    val = new SvValueExceptionFun( mTypeInt, mark() );
    return true;
    }
  return false;
  }

