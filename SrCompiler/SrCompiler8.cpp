/*
Проект "Компилятор"
Содержание
  Реализация класса TCompiler часть 3
  Функции:
*/
#include "SrCompiler.h"

using namespace SrCompiler6;
















SrType* SrCompiler::ConstExpression(int *intRes) {
  //Сохранить текущий указатель команды
  SrValuePtr val = 0;
  B2( val );
  if( val ) {
    SrType *res = mTypeFail;
    constValueCalc( val );
    if( val->isConst() ) {
      res = val->mType;
      *intRes = val->mConstInt;
      }
    delete val;
    return res;
    }
  return mTypeFail;
  }









void
SrCompiler::BExpression( SrValuePtr &val ) {
  B0( val );
  if( mToken == tsComma ) {
    SrValueComma *comma = new SrValueComma( val, mark() );
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
SrCompiler::B0( SrValuePtr &res ) {
  //Операция присваивания
  B1( res ); //Получить левую часть
  SrValueBinary *oper = 0;
  switch( mToken ) {
    case tsAssign    : oper = new SrValueStore( res, mark() );       break; // =
    case tsPlusAsg   : oper = new SrValueAddStore( res, mark() );    break; // +=
    case tsMinusAsg  : oper = new SrValueSubStore( res, mark() );    break; // -=
    case tsMulAsg    : oper = new SrValueMulStore( res, mark() );    break; // *=
    case tsDivAsg    : oper = new SrValueDivStore( res, mark() );    break; // /=
    case tsModAsg    : oper = new SrValueModStore( res, mark() );    break; // %=
    case tsXorAsg    : oper = new SrValueXorStore( res, mark() );    break; // ^=
    case tsAndAsg    : oper = new SrValueAndStore( res, mark() );    break; // &=
    case tsOrAsg     : oper = new SrValueOrStore( res, mark() );     break; // |=
    case tsLShiftAsg : oper = new SrValueLShiftStore( res, mark() ); break; // <<=
    case tsRShiftAsg : oper = new SrValueRShiftStore( res, mark() ); break; // >>=
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
SrCompiler::B1( SrValuePtr &val ) {
  B2( val );
  if( mToken == tsQuestion ) {
    SrValueCondition *cond = new SrValueCondition( val, mark() );
    val = cond;
    NextToken(); //Убрать ?
    B2(cond->mTrue);
    val->mType = cond->mTrue->mType;

    if( mToken != tsColon ) {
      ErrorEndSt( QObject::tr("Need :") );
      return;
      }

    B2(cond->mFalse);
    }
  }









/*
B2 операция ||
*/
void
SrCompiler::B2( SrValuePtr &val ) {
  B3( val );
  if( mToken == tsLOr ) {
    SrValueBinaryLong *orr = new SrValueLogOr( val, mTypeInt, mark() );
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
SrCompiler::B3( SrValuePtr &val ) {
  B4( val );
  if( mToken == tsLAnd ) {
    SrValueBinaryLong *andd = new SrValueLogAnd( val, mTypeInt, mark() );
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
SrCompiler::B4( SrValuePtr &val ) {
  B5( val );
  if( mToken == tsOr ) {
    SrValueBinaryLong *bin = new SrValueOr( val, mTypeInt, mark() );
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
SrCompiler::B5(SrValuePtr &val ) {
  B6( val );

  if( mToken == tsXor ) {
    SrValueBinaryLong *bin = new SrValueXor( val, mTypeInt, mark() );
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
SrCompiler::B6( SrValuePtr &val ) {
  B7( val );
  if( mToken == tsAnd ) {
    SrValueBinaryLong *bin = new SrValueAnd( val, mTypeInt, mark() );
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
SrCompiler::B7(SrValuePtr &val ) {
  B8( val );
  while( 1 ) {
    SrValueBinary *oper = 0;
    if( mToken == tsEqu )         oper = new SrValueEqu( val, mark() );
    else if( mToken == tsNotEqu ) oper = new SrValueNotEqu( val, mark() );
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
SrCompiler::B8(SrValuePtr &val ) {
  B9( val );
  while( 1 ) {
    SrValueBinary *oper = 0;
    switch( mToken ) {
      case tsLessEqu : oper = new SrValueLessEqu( val, mark() ); break;
      case tsGrowEqu : oper = new SrValueGreatEqu( val, mark() ); break;
      case tsLess    : oper = new SrValueLess( val, mark() ); break;
      case tsGrow    : oper = new SrValueGreat( val, mark() ); break;
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
SrCompiler::B9(SrValuePtr &val ) {
  B10( val );
  while( 1 ) {
    SrValueBinary *oper = 0;
    if( mToken == tsLShift )      oper = new SrValueLShift( val, mark() );
    else if( mToken == tsRShift ) oper = new SrValueRShift( val, mark() );
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
SrCompiler::B10(SrValuePtr &val ) {
  B11( val );
  while( 1 ) {
    SrValueBinary *oper = 0;
    if( mToken == tsPlus )       oper = new SrValueAdd( val, mark() );
    else if( mToken == tsMinus ) oper = new SrValueSub( val, mark() );
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
SrCompiler::B11( SrValuePtr &val ) {
  B12( val );
  while( 1 ) {
    SrValueBinary *oper = 0;
    if( mToken == tsMul )      oper = new SrValueMul( val, mark() );
    else if( mToken == tsDiv ) oper = new SrValueDiv( val, mark() );
    else if( mToken == tsMod ) oper = new SrValueMod( val, mark() );
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
SrCompiler::B12( SrValuePtr &val ) {
  switch( mToken ) {
    case tsInc :   // ++
      NextToken();
      B12( val );
      val = new SrValuePredInc( val, mark() );
      break;
    case tsDec :   // --
      NextToken();
      B12( val );
      val = new SrValuePredDec( val, mark() );
      break;
    case tsNot :   // ~
      NextToken();
      B12( val );
      val = new SrValueBitNot( val, mark() );
      break;
    case tsLNot :  // !
      NextToken();
      B12( val );
      val = new SrValueLogNot( val, mark() );
      break;
    case tsPlus :  // +
      NextToken();
      B12( val );
      break;
    case tsMinus : // -
      NextToken();
      B12( val );
      val = new SrValueNeg( val, mark() );
      break;
    case tsMul :   // *
      NextToken();
      B12( val );
      val = new SrValuePointer( val, mark() );
      break;
    case tsAnd :   // &
      NextToken();
      B12( val );
      val = new SrValueAddress( val, mark() );
      break;
    default :
      B13( val );
      if( mToken == tsInc ) {
        val = new SrValuePostInc( val, mark() );
        }
      else if( mToken == tsDec ) {
        val = new SrValuePostDec( val, mark() );
        }
      else break;
      NextToken();
    }
  }









/*
B13 операция [val] val() val. val->
*/
void
SrCompiler::B13(SrValuePtr &val ) {
  B15( val );
  while( 1 ) {

    if( mToken == tsSOpen ) {
      //Обработка массивов [val]
      SrValueArrayCell *array = new SrValueArrayCell( val, mark() );
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
      if( val->getClass() & TTYPE_FUNCTION  ) {
        SrFunctionType *fun = val->mType->toFunction();
        //Для члена структуры получаем адрес вызова
        //Инициализировать загрузку параметров
        //Вызов
        SrValueCall *call = new SrValueCall( val, fun->mResult, mark() );
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
        val = new SrValueConstInt( 0, mTypeInt, mark() );
        }
      }
    else if( mToken == tsPoint ) {
      //Достать член структуры
      NextToken();
      //Операция доставания
      B14Member( val );
      }
    else if( mToken == tsRef ) {
      if( val->getType() && val->mType->mClass == TTYPE_POINTER && val->mType->mBaseType->mClass & (TTYPE_STRUCT | TTYPE_OBJECT) ) {
        val = new SrValuePointer( val, mark() );
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
void SrCompiler::B14Member(SrValuePtr &val)
  {
  if( val->getClass() & (TTYPE_STRUCT | TTYPE_OBJECT) ) {
    SrStruct *cls = val->mType->toStruct();
    if( mToken == ttName ) {
      //Получено имя, проверим, является ли оно членом данной структуры
      SrVariable *var = cls->getMember(mToken.mString);
      if( var ) val = new SrValueMemberVariable( var, val, mark() );
      else {
        //Проверить среди функций
        SrFunction *fun = cls->getFunction( mToken.mString );
        if( fun ) val = new SvValueMemberFunction( fun, val, mark() );
        else ErrorEndSt( QObject::tr("Error. \"%1\" not member of struct \"%2\"").arg(mToken.mString).arg( cls->mName ) );
        }
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
SrCompiler::B15( SrValuePtr &val ) {
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
      if( BoWaitFunction(val) )
        break;
      SrVariable *var = 0;
      SrFunction *fun = 0;
      //Если активна функция-член, то пробуем искать член структуры
      if( mActiveStruct ) {
        var = mActiveStruct->getMember( mToken.mString );
        fun = mActiveStruct->getFunction( mToken.mString );
        }

      //Если не найдено, то ищем по иерархии блоков
      if( var == 0 && fun == 0 && mContext )
        var = mContext->getLocal( mToken.mString );

      //Если ничего не найдено и активна функция, то ищем по ее параметрам
      if( var == 0 && fun == 0 && mActiveFunction )
        var = mActiveFunction->getParam( mToken.mString );

      //Если ничего не найдено, то ищем по глобальным спискам
      if( var == 0 && fun == 0 ) {
        var = mVarGlobal.mHash.value( mToken.mString, 0 );
        fun = mFunGlobal.mHash.value( mToken.mString, 0 );
        }

      //Если и теперь ничего найдено, то для сцены просто фиксируем имя
      if( var == 0 && fun == 0 )
        if( mActiveScene ) {
          QString fullName = mToken.mString;
          NextToken();
          while( mToken == tsPoint ) {
            NextToken();
            if( mToken == ttName ) {
              fullName.append( QChar('.') );
              fullName.append( mToken.mString );
              NextToken();
              }
            else {
              //За точкой не следует имя
              //Выдать ошибку
              ErrorEndSt( QObject::tr("Error. After \'.\' must be name.") );
              }
            }
          //В fullName находится ссылочный путь, разместить его в свойства
          SvSmlProperty *prop = 0;
          if( mActiveScene->mProperties.mHash.contains(fullName) )
            //Присутствует в активной сцене в виде свойства
            prop = mActiveScene->mProperties.mHash.value(fullName,0);
          else {
            //Ссылка вперед, проверить наличие типа
            SrType *type = mTypeInt;
            if( mToken == tsDColon ) {
              //Идет указание типа
              NextToken();
              if( mToken != ttType ) {
                //Выдать ошибку
                Error( QObject::tr("Error. Need type name.") ); // Неопознанный тип
                val = new SrValueConstInt( 0, mTypeInt, mark() );
                return;
                }
              type = mToken.mType;
              }
            prop = mActiveScene->needAlias( fullName, type, mark() );
            if( prop == 0 ) {
              //Выдать ошибку
              Error( QObject::tr("Error. Type mismatch in redeclaration %1.").arg(fullName) ); // Несовпадение типа при переопределении
              val = new SrValueConstInt( 0, mTypeInt, mark() );
              return;
              }
            }
          val = new SvValueProperty( prop, mark() );
          return;
          }
        else {
          //Вообще ничего не найдено
          //Выдать ошибку
          Error( QObject::tr("Error. Undefined identifier \"%1\".").arg(mToken.mString) ); // Неопределенный идентификатор
          //Заполнить val константой
          val = new SrValueConstInt( 0, mTypeInt, mark() );
          }
      else {
        if( var ) val = new SrValueVariable( var, mark() );
        else      val = new SrValueFunction( fun, mark() );
        }

      }
      break;
    case ttInt :
      //val - целая константа
      val = new SrValueConstInt( mToken.mIValue, mTypeInt, mark() );
      break;
    case ttFloat :
      //val - константа с плавающей точкой
      errorInLine( QObject::tr("Error. Float unsupported"), mark() );
      val = new SrValueConstInt( 0, mTypeInt, mark() );
      break;
    case ttString :
      //val - индекс строки в таблице строк
      val = new SrValueConstString( mStringTable.count(), mTypeCString, mark() );
      mStringTable.append( mToken.mString );
      break;
    default :
      ErrorEndSt( QObject::tr("Error. Undefined context.") );
      return;
    }
  NextToken();
  }







bool
SrCompiler::BoWaitFunction( SrValuePtr &val) {
  if( mToken.mString == QString("Wait") ) {
    NextToken();
    if( mToken != tsOpen ) {
      ErrorEndSt( Need('(') );
      return true;
      }
    NextToken();
    if( mToken != tsClose ) {
      ErrorEndSt( Need(')') );
      return true;
      }
    //Генерировать
    val = new SrValueWaitFun( mTypeVoid, mark() );
    return true;
    }
  return false;
  }

