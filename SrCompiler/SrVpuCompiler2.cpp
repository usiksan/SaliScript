/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#include "SrVpuCompiler.h"
#include <QDebug>

using namespace SrCompiler6;

void SrVpuCompiler::gValue(SrProgramm *prog, SrValue *val, bool keepValue, bool address)
  {
  if( val == 0 ) return;
  switch( val->code() ) {

    case svvVariable :  //Переменная (глобальная, локальная, параметр, член от this)
      //Загрузить значение переменной
      gvvVariable( prog, dynamic_cast<SrValueVariable*>(val), keepValue, address );
      break;

    case svvFunction :    //Функция (глобальная, член от this)
      //Загрузить адрес функции
      gvvFunction( prog, dynamic_cast<SrValueFunction*>(val), keepValue, address );
      break;

    case svvWaitFun :   //Специальная функция wait
      //Вызвать функцию ожидания
      prog->addCode( VPC1_WAIT, val->mMark );
      codePrint( QString("VPC1_WAIT") );
      break;

    case svvMemberVariable :  //Член от произвольной структуры структуры
      gvvMemberVariable( prog, dynamic_cast<SrValueMemberVariable*>(val), keepValue, address );
      break;

    case svvConstInt :      //Константа
      gvvConstInt( prog, dynamic_cast<SrValueConstInt*>(val), keepValue, address );
      break;

    case svvConstString :
      gvvConstString( prog, dynamic_cast<SrValueConstString*>(val), keepValue, address );
      break;

    case svvPointer :
      gvvPointer( prog, dynamic_cast<SrValuePointer*>(val), keepValue, address );
      break;

    case svvAddress :
      gvvAddress( prog, dynamic_cast<SrValueAddress*>(val), keepValue, address );
      break;

    case svvPredInc :
      gvvPredInc( prog, dynamic_cast<SrValuePredInc*>(val), keepValue, address );
      break;

    case svvPredDec :
      gvvPredDec( prog, dynamic_cast<SrValuePredDec*>(val), keepValue, address );
      break;

    case svvPostInc :
      gvvPostInc( prog, dynamic_cast<SrValuePostInc*>(val), keepValue, address );
      break;

    case svvPostDec :
      gvvPostDec( prog, dynamic_cast<SrValuePostDec*>(val), keepValue, address );
      break;

    case svvNot :
      gvvNot( prog, dynamic_cast<SrValueBitNot*>(val), keepValue, address );
      break;

    case svvLogNot :
      gvvLogNot( prog, dynamic_cast<SrValueLogNot*>(val), keepValue, address );
      break;

    case svvNeg :
      gvvNeg( prog, dynamic_cast<SrValueNeg*>(val), keepValue, address );
      break;

    case svvStore :
      gvvStore( prog, dynamic_cast<SrValueStore*>(val), keepValue, address );
      break;

    case svvArrayCell :      //Операция индекса массива
      gvvArrayCell( prog, dynamic_cast<SrValueArrayCell*>(val), keepValue, address );
      break;

    case svvMul :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_MUL, QString("VPC1_MUL") );
      break;

    case svvMulStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_MUL, QString("VPC1_MUL") );
      break;

    case svvDiv :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_DIV, QString("VPC1_DIV") );
      break;

    case svvDivStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_DIV, QString("VPC1_DIV") );
      break;

    case svvMod :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_MOD, QString("VPC1_MOD") );
      break;

    case svvModStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_MOD, QString("VPC1_MOD") );
      break;

    case svvAdd :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_ADD, QString("VPC1_ADD") );
      break;

    case svvAddStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_ADD, QString("VPC1_ADD") );
      break;

    case svvSub :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_SUB, QString("VPC1_SUB") );
      break;

    case svvSubStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_SUB, QString("VPC1_SUB") );
      break;

    case svvLShift :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_LSHIFT, QString("VPC1_LSHIFT") );
      break;

    case svvLShiftStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_LSHIFT, QString("VPC1_LSHIFT") );
      break;

    case svvRShift :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_RSHIFT, QString("VPC1_RSHIFT") );
      break;

    case svvRShiftStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_RSHIFT, QString("VPC1_RSHIFT") );
      break;

    case svvOrStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_OR, QString("VPC1_OR") );
      break;

    case svvAndStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_AND, QString("VPC1_AND") );
      break;

    case svvXorStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_XOR, QString("VPC1_XOR") );
      break;

    case svvEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_EQU, QString("VPC1_EQU") );
      break;

    case svvNotEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_NOT_EQU, QString("VPC1_NOT_EQU") );
      break;

    case svvLessEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_LESS_EQU, QString("VPC1_LESS_EQU") );
      break;

    case svvLess :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_LESS, QString("VPC1_LESS") );
      break;

    case svvGreat :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_GREAT, QString("VPC1_GREAT") );
      break;

    case svvGreatEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VPC1_GREAT_EQU, QString("VPC1_GREAT_EQU") );
      break;

    case svvLogAnd :
      gvvLogAnd( prog, dynamic_cast<SrValueLogAnd*>(val), keepValue, address );
      break;

    case svvLogOr :
      gvvLogOr( prog, dynamic_cast<SrValueLogOr*>(val), keepValue, address );
      break;

    case svvOr :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VPC1_OR, QString("VPC1_OR") );
      break;

    case svvAnd :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VPC1_AND, QString("VPC1_AND") );
      break;

    case svvXor :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VPC1_XOR, QString("VPC1_XOR") );
      break;

    case svvComma :      //Операция ,
      gvvComma( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address );
      break;

    case svvCondition :      //Тройная условная операция
      gvvCondition( prog, dynamic_cast<SrValueCondition*>(val), keepValue, address );
      break;

    case svvCall :      //Вызов функции
      gvvCall( prog, dynamic_cast<SrValueCall*>(val), keepValue, address );
      break;

    case svvLast :
      break;
    }

  }







//Загрузить значение
void SrVpuCompiler::gvvVariable(SrProgramm *prog, SrValueVariable *var, bool keepValue, bool address)
  {
  if( var == 0 ) {
    Error( QObject::tr("Error. gvvVariable") );
    return;
    }
  if( var->mVariable == 0 ) {
    var->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Undefined variable"), var->mMark );
    return;
    }

  if( var->mConst ) {
    if( keepValue )
      gPushConst( prog, var->mConstInt, var->mMark );
    }
  else {

    //Проверить на массив
    if( (address || var->mVariable->mType->isArray() || var->mVariable->mType->isStruct()) && keepValue ) {
      switch ( var->mVariable->mSort ) {
        case tsrGlobal :
          //Глобальная переменная
          prog->addCodeParam24( VPC4_PUSH_CONST, var->mVariable->mAddress, var->mMark );
          codePrint( QString("VPC4_PUSH_CONST ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
          var->mConst = true;
          var->mConstInt = var->mVariable->mAddress;
          break;

        case tsrLocal :
          //Локальная переменная
          prog->addCodeParam8( VPC2_PUSH_B_OFFSET, var->mVariable->mAddress, var->mMark );
          codePrint( QString("VPC2_PUSH_B_OFFSET ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
          break;

        case tsrMember :
          prog->addCodeParam8( VPC2_PUSH_T_OFFSET, var->mVariable->mAddress, var->mMark );
          codePrint( QString("VPC2_PUSH_T_OFFSET ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
          break;
        }
      }
    else if( keepValue ) {
      switch ( var->mVariable->mSort ) {
        case tsrGlobal :
          //Глобальная переменная
          if( var->mVariable->mType->isObject() ) {
            prog->addCodeParam24( VPC4_OBJ_PUSH_GLOBAL, var->mVariable->mAddress, var->mMark );
            codePrint( QString("VPC4_OBJ_PUSH_GLOBAL ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
            }
          else {
            prog->addCodeParam24( VPC4_PUSH_GLOBAL, var->mVariable->mAddress, var->mMark );
            codePrint( QString("VPC4_PUSH_GLOBAL ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
            }
          break;

        case tsrLocal :
          //Локальная переменная
          if( var->mVariable->mType->isObject() ) {
            prog->addCodeParam8( VPC2_OBJ_PUSH_LOCAL, var->mVariable->mAddress, var->mMark );
            codePrint( QString("VPC2_OBJ_PUSH_LOCAL ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
            }
          else {
            prog->addCodeParam8( VPC2_PUSH_LOCAL, var->mVariable->mAddress, var->mMark );
            codePrint( QString("VPC2_PUSH_LOCAL ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
            }
          break;

        case tsrMember :
          prog->addCodeParam8( VPC2_PUSH_MEMBER, var->mVariable->mAddress, var->mMark );
          codePrint( QString("VPC2_PUSH_MEMBER ") + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
          break;
        }
      }

    //Разобраться с типами
    if( address ) {
      //В стеке указатель
      //Для массива - оставляем тип массива
      if( var->mVariable->mType->isArray() )
        var->mType = var->mVariable->mType;
      else
        //В остальных случаях - формируем указатель
        var->mType = mTypeList.getTypePointer(var->mVariable->mType);
      }
    else
      //Тип переменной
      var->mType = var->mVariable->mType;

    }

  }





void SrVpuCompiler::gvvProperty(SrProgramm *prog, SrValueProperty *prop, bool keepValue, bool address)
  {
  if( prop == 0 ) {
    Error( QObject::tr("Error. gvvProperty") );
    return;
    }
  if( prop->mProperty == 0 ) {
    prop->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Undefined property"), prop->mMark );
    return;
    }
  if( keepValue ) {
    int addr = prop->mProperty->mIndex;
    if( address ) {
      prog->addCodeParam32( VPC5_PROP_PUSH_ADDR, addr, prop->mMark );
      codePrint( QString("VPC5_PUSH_PROP_ADDR ") + prop->mProperty->mName + QString("<%1>").arg(prop->mProperty->mIndex) );
      }
    else {
      prog->addCodeParam32( VPC5_PROP_PUSH, addr, prop->mMark );
      codePrint( QString("VPC5_PUSH_PROP ") + prop->mProperty->mName + QString("<%1>").arg(prop->mProperty->mIndex) );
      }
    }

  //Разобраться с типами
  if( address )
    prop->mType = prop->mProperty->mType->getTypePropPointer();
  else
    prop->mType = prop->mProperty->mType;
  }






//Загрузить адрес функции
void SrVpuCompiler::gvvFunction(SrProgramm *prog, SrValueFunction *fun, bool keepValue, bool address)
  {
  if( fun == 0 ) {
    Error( QObject::tr("Error. gvvFunction") );
    return;
    }
  fun->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), fun->mMark );
    return;
    }
  if( fun->mFunction == 0 ) {
    errorInLine( QObject::tr("Error. Undefined function"), fun->mMark );
    return;
    }

  fun->mType = fun->mFunction->mType;

  if( keepValue ) {

    if( fun->mFunction->mImportIndex ) {
      //Импортная функция
      prog->addCodeParam32( VPC5_PUSH_CONST, fun->mFunction->mImportIndex | 0x80000000, fun->mMark );
      codePrint( QString("VPC5_PUSH_CONST %1 [%2]").arg(fun->mFunction->mName).arg(fun->mFunction->mImportIndex) );
      fun->mConst = true;
      fun->mConstInt = fun->mFunction->mImportIndex | 0x80000000;
      }

    else if( fun->mFunction->mMemberIndex ) {
      //Функция-член от this
      prog->addCodeParam16( VPC3_PUSH_T_FUNCTION, fun->mFunction->mMemberIndex, fun->mMark );
      codePrint( QString("VPC3_PUSH_T_FUNCTION %1 [%2]").arg(fun->mFunction->mName).arg(fun->mFunction->mMemberIndex) );
      }

    else {
      //Глобальная функция
      prog->addCodeParam24( VPC4_PUSH_CONST, fun->mFunction->mAddress, fun->mMark );
      codePrint( QString("VPC4_PUSH_CONST %1 [%2]").arg(fun->mFunction->mName).arg(fun->mFunction->mAddress) );
      fun->mConst = true;
      fun->mConstInt = fun->mFunction->mAddress;
      }
    }
  }






void SrVpuCompiler::gvvConstInt(SrProgramm *prog, SrValueConstInt *cnst, bool keepValue, bool address)
  {
  if( cnst == 0 ) {
    Error( QObject::tr("Error. gvvConstInt") );
    return;
    }
  cnst->mType = mTypeInt;
  cnst->mConstInt = cnst->mIValue;
  cnst->mConst = true;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), cnst->mMark );
    return;
    }
  if( keepValue ) {
    //Загружаем константное значение
    gPushConst( prog, cnst->mIValue, cnst->mMark );
    }
  }






void SrVpuCompiler::gvvConstString(SrProgramm *prog, SrValueConstString *cnst, bool keepValue, bool address)
  {
  if( cnst == 0 ) {
    Error( QObject::tr("Error. gvvConstString") );
    return;
    }
  cnst->mType = mTypeCString;
  cnst->mConstInt = cnst->mIndex;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), cnst->mMark );
    return;
    }
  if( keepValue ) {
    //Загружаем константное значение
    prog->addCodeParam24( VPC4_PUSH_CSTRING, cnst->mIndex, cnst->mMark );
    codePrint( QString("VPC4_PUSH_CSTRING %1 [%2]").arg(cnst->mIndex).arg(mStringTable.at(cnst->mIndex)) );
    }
  }





void SrVpuCompiler::gvvPointer(SrProgramm *prog, SrValuePointer *ptr, bool keepValue, bool address)
  {
  if( ptr == 0 ) {
    Error( QObject::tr("Error. gvvPointer") );
    return;
    }
  ptr->mType = mTypeInt;
  if( ptr->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined pointer to"), ptr->mMark );
    return;
    }

  gValue( prog, ptr->mOperand, keepValue, false );

  //Разобраться с типами
  ptr->mType = ptr->mOperand->mType;
  if( !address )
    ptr->mType = ptr->mType->mBaseType;

  //Убедиться, что имеем дело с указателем
  if( !ptr->mOperand->mType->isPointer() )
    errorInLine( QObject::tr("Error. Pointer not allowed in this context"), ptr->mMark );

  if( keepValue ) {
    if( !address && !ptr->mType->isStruct() ) {
      //Нужно само значение по указателю
      gLoad( prog, ptr );
      }
    }
  }






void SrVpuCompiler::gvvMemberVariable(SrProgramm *prog, SrValueMemberVariable *var, bool keepValue, bool address)
  {
  if( var == 0 ) {
    Error( QObject::tr("Error. gvvMemberVariable") );
    return;
    }
  var->mType = mTypeInt;
  if( var->mVariable == 0 ) {
    errorInLine( QObject::tr("Error. Undefined variable"), var->mMark );
    return;
    }

  //Если структура имеет константный адрес, то сразу все вычисляем
  if( var->mStruct->isConst() ) {
    if( keepValue ) {
      int addr = var->mStruct->mConstInt + var->mVariable->mAddress;
      if( address )
        gPushConst( prog, addr, var->mMark );
      else {
        //Глобальная переменная
        prog->addCodeParam24( VPC4_PUSH_GLOBAL, addr, var->mMark );
        codePrint( QString("VPC4_PUSH_GLOBAL %1.").arg(var->mStruct->mConstInt) + var->mVariable->mName + QString("<%1>").arg( var->mVariable->mAddress ) );
        }
      }
    }
  else {
    //Получить значение указателя
    gValue( prog, var->mStruct, keepValue, true );
    if( keepValue ) {
      //Суммируем с адресом члена
      gPushConst( prog, var->mVariable->mAddress, var->mMark );
      prog->addCode( VPC1_ADD, var->mMark );
      codePrint( QString("VPC1_AAD") );
      if( !address ) {
        //Нужно само значение по указателю
        gLoad( prog, var );
        }
      }
    }

  //Разобраться с типами
  if( address )
    //Нужен адрес
    var->mType = mTypeList.getTypePointer(var->mVariable->mType);
  else
    //Нужно значение
    var->mType = var->mVariable->mType;
  }




void SrVpuCompiler::gvvMemberFunction(SrProgramm *prog, SrValueMemberFunction *fun, bool keepValue, bool address)
  {
  if( fun == 0 ) {
    Error( QObject::tr("Error. gvvMemberFunction") );
    return;
    }
  fun->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), fun->mMark );
    return;
    }
  if( fun->mFunction == 0 || fun->mStruct == 0 ) {
    errorInLine( QObject::tr("Error. Undefined function"), fun->mMark );
    return;
    }

  fun->mType = fun->mFunction->mType;

  //Получить значение указателя
  gValue( prog, fun->mStruct, keepValue, false );

  if( keepValue ) {
    if( fun->mFunction->mImportIndex ) {
      //Сформировать указатель на функцию-член объекта
      prog->addCodeParam8( VPC2_OBJ_ADD_FMEMBER, fun->mFunction->mImportIndex, fun->mMark );
      codePrint( QString("VPC2_OBJ_ADD_FMEMBER %1 [%2]").arg(fun->mFunction->mName).arg(fun->mFunction->mImportIndex) );
      }
    else if( fun->mFunction->mMemberIndex == 0 )
      errorInLine( QObject::tr("Error. Not member function"), fun->mMark );
    else {
      //Сформировать указатель на функцию-член
      prog->addCodeParam8( VPC2_ADD_FMEMBER, fun->mFunction->mMemberIndex, fun->mMark );
      codePrint( QString("VPC2_ADD_FMEMBER %1 [%2]").arg(fun->mFunction->mName).arg(fun->mFunction->mMemberIndex) );
      }
    }
  }



void SrVpuCompiler::gvvAddress(SrProgramm *prog, SrValueAddress *addr, bool keepValue, bool address)
  {
  if( addr == 0 ) {
    Error( QObject::tr("Error. gvvAddress") );
    return;
    }
  addr->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), addr->mMark );
    return;
    }

  if( addr->mConst ) {
    if( keepValue )
      gPushConst( prog, addr->mConstInt, addr->mMark );
    }
  else {
    gValue( prog, addr->mOperand, keepValue, true );
    addr->mType = addr->mOperand->mType;
    addr->mConst = addr->mOperand->mConst;
    if( addr->mConst )
      addr->mConstInt = addr->mOperand->mConstInt;

    //Если поступил не указатель, то взятие адреса не удалось
    if( !addr->mType->isPointer() )
      errorInLine( QObject::tr("Error. Cant take address. Undefined context."), addr->mMark );
    }
  }







void SrVpuCompiler::gvvPredInc(SrProgramm *prog, SrValuePredInc *inc, bool keepValue, bool address)
  {
  if( inc == 0 ) {
    Error( QObject::tr("Error. gvvPredInc") );
    return;
    }
  inc->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), inc->mMark );
    return;
    }
  if( inc->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, inc->mOperand, true, true );

  if( inc->mOperand->mType->isPointer() && !inc->mOperand->mType->mBaseType->isObject() )
    inc->mType = inc->mOperand->mType->mBaseType;
  else {
    ErrorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark.mFile, inc->mMark.mLine );
    }


  if( inc->mType->isInt() ) {
    //Для целых чисел инкремент на 1
    if( keepValue ) {
      prog->addCode( VPC1_PRED_INC, inc->mMark );
      codePrint( QString("VPC1_PRED_INC") );
      }
    else {
      prog->addCode( VPC1_INC, inc->mMark );
      codePrint( QString("VPC1_INC") );
      }
    }
  else if( inc->mType->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VPC4_PRED_INC, inc->mType->mBaseType->mSize, inc->mMark );
      codePrint( QString("VPC4_PRED_INC %1").arg(inc->mType->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VPC4_INC, inc->mType->mBaseType->mSize, inc->mMark );
      codePrint( QString("VPC4_INC %1").arg(inc->mType->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred increment not allowed with this type"), inc->mMark );

  }






void SrVpuCompiler::gvvPredDec(SrProgramm *prog, SrValuePredDec *dec, bool keepValue, bool address)
  {
  if( dec == 0 ) {
    Error( QObject::tr("Error. gvvPredInc") );
    return;
    }
  dec->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), dec->mMark );
    return;
    }
  if( dec->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, dec->mOperand, true, true );

  if( dec->mOperand->mType->isPointer() && !dec->mOperand->mType->mBaseType->isObject() )
    dec->mType = dec->mOperand->mType->mBaseType;
  else {
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );
    }


  if( dec->mType->isInt() ) {
    //Для целых чисел декремент на 1
    if( keepValue ) {
      prog->addCode( VPC1_PRED_DEC, dec->mMark );
      codePrint( QString("VPC1_PRED_DEC") );
      }
    else {
      prog->addCode( VPC1_DEC, dec->mMark );
      codePrint( QString("VPC1_DEC") );
      }
    }
  else if( dec->mType->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VPC4_PRED_DEC, dec->mType->mBaseType->mSize, dec->mMark );
      codePrint( QString("VPC4_PRED_DEC %1").arg(dec->mType->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VPC4_DEC, dec->mType->mBaseType->mSize, dec->mMark );
      codePrint( QString("VPC4_DEC %1").arg(dec->mType->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred decrement not allowed with this type"), dec->mMark );

  }




void SrVpuCompiler::gvvPostInc(SrProgramm *prog, SrValuePostInc *inc, bool keepValue, bool address)
  {
  if( inc == 0 ) {
    Error( QObject::tr("Error. gvvPredInc") );
    return;
    }
  inc->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), inc->mMark );
    return;
    }
  if( inc->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, inc->mOperand, true, true );

  if( inc->mOperand->mType->isPointer() && !inc->mOperand->mType->mBaseType->isObject() )
    inc->mType = inc->mOperand->mType->mBaseType;
  else {
    errorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark );
    }

  if( inc->mType->isInt() ) {
    //Для целых чисел инкремент на 1
    if( keepValue ) {
      prog->addCode( VPC1_POST_INC, inc->mMark );
      codePrint( QString("VPC1_POST_INC") );
      }
    else {
      prog->addCode( VPC1_INC, inc->mMark );
      codePrint( QString("VPC1_INC") );
      }
    }
  else if( inc->mType->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VPC4_POST_INC, inc->mType->mBaseType->mSize, inc->mMark );
      codePrint( QString("VPC4_POST_INC %1").arg(inc->mType->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VPC4_INC, inc->mType->mBaseType->mSize, inc->mMark );
      codePrint( QString("VPC4_INC %1").arg(inc->mType->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Post increment not allowed with this type"), inc->mMark );
  }




void SrVpuCompiler::gvvPostDec(SrProgramm *prog, SrValuePostDec *dec, bool keepValue, bool address)
  {
  if( dec == 0 ) {
    Error( QObject::tr("Error. gvvPredInc") );
    return;
    }
  dec->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), dec->mMark );
    return;
    }
  if( dec->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, dec->mOperand, true, true );

  if( dec->mOperand->mType->isPointer() && !dec->mOperand->mType->mBaseType->isObject() )
    dec->mType = dec->mOperand->mType->mBaseType;
  else {
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );
    }


  if( dec->mType->isInt() ) {
    //Для целых чисел декремент на 1
    if( keepValue ) {
      prog->addCode( VPC1_POST_DEC, dec->mMark );
      codePrint( QString("VPC1_POST_DEC") );
      }
    else {
      prog->addCode( VPC1_DEC, dec->mMark );
      codePrint( QString("VPC1_DEC") );
      }
    }
  else if( dec->mType->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VPC4_POST_DEC, dec->mType->mBaseType->mSize, dec->mMark );
      codePrint( QString("VPC4_POST_DEC %1").arg(dec->mType->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VPC4_DEC, dec->mType->mBaseType->mSize, dec->mMark );
      codePrint( QString("VPC4_DEC %1").arg(dec->mType->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred decrement not allowed with this type"), dec->mMark );

  }






void SrVpuCompiler::gvvNot(SrProgramm *prog, SrValueBitNot *vnot, bool keepValue, bool address)
  {
  if( vnot == 0 ) {
    Error( QObject::tr("Error. gvvNot") );
    return;
    }
  vnot->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), vnot->mMark );
    return;
    }
  if( vnot->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand not to"), vnot->mMark );
    return;
    }

  if( vnot->mConst ) {
    if( keepValue )
      gPushConst( prog, vnot->mConstInt, vnot->mMark );
    }
  else {
    //Получить значение
    gValue( prog, vnot->mOperand, keepValue, false );

    //Тип результата тот-же, что и у операнда
    vnot->mType = vnot->mOperand->mType;

    //Разобраться с константой
    vnot->checkConst();

    //Если операнд не целого типа - то ошибка
    if( !vnot->mType->isInt() )
      errorInLine( QObject::tr("Error. Not allowed only with int"), vnot->mMark );

    if( keepValue ) {
      prog->addCode( VPC1_NOT, vnot->mMark );
      codePrint( QString("VPC1_NOT") );
      }
    }
  }





void SrVpuCompiler::gvvLogNot(SrProgramm *prog, SrValueLogNot *logNot, bool keepValue, bool address)
  {
  if( logNot == 0 ) {
    Error( QObject::tr("Error. gvvLogNot") );
    return;
    }
  //Тип результата - все время int
  logNot->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), logNot->mMark );
    return;
    }
  if( logNot->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand not to"), logNot->mMark );
    return;
    }

  if( logNot->mConst ) {
    if( keepValue )
      gPushConst( prog, logNot->mConstInt, logNot->mMark );
    }
  else {
    //Получить значение
    gValue( prog, logNot->mOperand, keepValue, false );

    //Разобраться с константой
    logNot->checkConst();

    if( !(logNot->mOperand->mType->isInt() || logNot->mOperand->mType->isPointer()) ) {
      errorInLine( QObject::tr("Error. LogNot allowed only with int and pointer"), logNot->mMark );
      }

    if( keepValue ) {
      prog->addCode( VPC1_LNOT, logNot->mMark );
      codePrint( QString("VPC1_LNOT") );
      }
    }
  }




void SrVpuCompiler::gvvNeg(SrProgramm *prog, SrValueNeg *neg, bool keepValue, bool address)
  {
  if( neg == 0 ) {
    Error( QObject::tr("Error. gvvNeg") );
    return;
    }
  //Тип результата - все время int
  neg->mType = mTypeInt;
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), neg->mMark );
    return;
    }
  if( neg->mOperand == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand neg to"), neg->mMark );
    return;
    }

  if( neg->mConst ) {
    if( keepValue )
      gPushConst( prog, neg->mConstInt, neg->mMark );
    }
  else {
    //Получить значение
    gValue( prog, neg->mOperand, keepValue, false );

    //Разобраться с константой
    neg->checkConst();

    //Если операнд не целого типа - то ошибка
    if( !neg->mType->isInt() )
      errorInLine( QObject::tr("Error. NEG allowed only with int"), neg->mMark );

    if( keepValue ) {
      prog->addCode( VPC1_NEG, neg->mMark );
      codePrint( QString("VPC1_NEG") );
      }
    }
  }




void SrVpuCompiler::gvvStore(SrProgramm *prog, SrValueStore *store, bool keepValue, bool address)
  {
  if( store == 0 ) {
     Error( QObject::tr("Error. gvvStore") );
     return;
     }
   store->mType = mTypeInt;
   if( address ) {
     errorInLine( QObject::tr("Error. Need LValue"), store->mMark );
     return;
     }
   if( store->mOperand1 == 0 || store->mOperand2 == 0 ) {
     errorInLine( QObject::tr("Error. Undefined operand store to"), store->mMark );
     return;
     }
  //Для той, куда сохраняем берем адрес
  gValue( prog, store->mOperand1, true, true );
  //Теперь значение
  gValue( prog, store->mOperand2, true, false );

  //Тип результата соответствует типу операнда 2
  store->mType = store->mOperand2->mType;

  //Проверить возможность сохранения
  //Сохранять можем только по указателю
  if( !store->mOperand1->mType->isPointer() || !store->mOperand1->mType->mBaseType->canAssign( store->mOperand2->mType ) ) {
    errorInLine( QObject::tr("Error. Can't store %1 to %2").arg(store->mOperand2->mType->mName).arg(store->mOperand1->mType->mBaseType->mName), store->mMark );
    return;
    }
  if( keepValue ) {
    if( store->mOperand1->mType->isPropPtr() ) {
      //Идет сохранение в свойство
      prog->addCode( VPC1_PROP_STORE, store->mMark );
      codePrint( QString("VPC1_PROP_STORE") );
      }
    else if( store->mOperand2->mType->isObject() ) {
      //Сохранение объекта
      prog->addCode( VPC1_OBJ_STORE, store->mMark );
      codePrint( QString("VPC1_OBJ_STORE") );
      }
    else {
      //Сохранение значения
      prog->addCode( VPC1_STORE, store->mMark );
      codePrint( QString("VPC1_STORE") );
      }
    }
  else {
    if( store->mOperand1->mType->isPropPtr() ) {
      //Идет сохранение в свойство
      prog->addCode( VPC1_PROP_POP, store->mMark );
      codePrint( QString("VPC1_PROP_POP") );
      }
    else if( store->mOperand2->mType->isObject() ) {
      //Сохранение объекта
      prog->addCode( VPC1_OBJ_POP, store->mMark );
      codePrint( QString("VPC1_OBJ_POP") );
      }
    else {
      //Сохранение значения
      prog->addCode( VPC1_POP, store->mMark );
      codePrint( QString("VPC1_POP") );
      }
    }
  }




void SrVpuCompiler::gvvArrayCell(SrProgramm *prog, SrValueArrayCell *array, bool keepValue, bool address)
  {
  if( array == 0 ) {
    Error( QObject::tr("Error. gvvArrayCell") );
    return;
    }
  array->mType = mTypeInt;
  if( array->mOperand1 == 0 || array->mOperand2 == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand array to"), array->mMark );
    return;
    }
  if( array->mConst ) {
    if( keepValue )
      gPushConst( prog, array->mConstInt, array->mMark );
    }
  else {
    //Получить адрес массива
    gValue( prog, array->mOperand1, keepValue, false );
    //Ячейку массива можем получать только у массива и указателя
    if( !(array->mOperand1->mType->isArray() || array->mOperand1->mType->isPointer()) )
      errorInLine( QObject::tr("Error. Index allowed only for arrays and pointers"), array->mMark );
    else {
      //Разобраться с типом
      if( address )
        array->mType = mTypeList.getTypePointer( array->mOperand1->mType->mBaseType );
      else
        array->mType = array->mOperand1->mType->mBaseType;
      }

    //Получить индекс
    gValue( prog, array->mOperand2, keepValue, false );

    //Разобраться с константой
    if( address && array->mOperand1->mConst && array->mOperand2->mConst ) {
      array->mConst = true;
      array->mConstInt = array->mOperand1->mConstInt + array->mOperand2->mConstInt * array->mOperand1->mType->mBaseType->mSize;
      }

    if( keepValue ) {
      if( array->mOperand1->mType->mBaseType->mSize > 1 ) {
        //Умножить на размер ячейки
        gPushConst( prog, array->mOperand1->mType->mBaseType->mSize, array->mMark );
        prog->addCode( VPC1_MUL, array->mMark );
        codePrint( QString("VPC1_MUL") );
        }
      prog->addCode( VPC1_ADD, array->mMark );
      codePrint( QString("VPC1_ADD") );
      if( !address )
        gLoad( prog, array );
      }
    }
  }




void SrVpuCompiler::gvvBinaryStore(SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvBinaryStore") );
    return;
    }
  binary->mType = mTypeInt;
  if( binary->mOperand1 == 0 || binary->mOperand2 == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand binaryStore to"), binary->mMark );
    return;
    }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  //Получить операнды в стеке и выполнить операцию
  gValue( prog, binary->mOperand1, true, true );
  //Тип результата соответсвует тому, куда указывает операнд 1
  binary->mType = binary->mOperand1->mType->mBaseType;
  //Сдублировать адрес
  prog->addCode( VPC1_PUSH_TOS, binary->mMark );
  codePrint( QString("VPC1_PUSH_TOS") );

  //Получить значение для операции
  gLoad( prog, binary );

  //Второй операнд
  gValue( prog, binary->mOperand2, true, false );

  //Проверить типы операндов
  if( binary->mOperand1->mType->isPointer() && binary->mType->isInt() && binary->mOperand2->mType->isInt() ) {
    //Операнды целые

    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrint( codeList );
    }
  else if( code == VPC1_ADD && binary->mOperand1->mType->isPointer() && binary->mType->isPointer() && binary->mOperand2->mType->isInt() ) {
    //Операция сложения указателя с целым
    if( binary->mType->mBaseType->mSize > 1 ) {
      //Умножить на размер элемента
      gPushConst( prog, binary->mType->mBaseType->mSize, binary->mMark );
      prog->addCode( VPC1_MUL, binary->mMark );
      codePrint( QString("VPC1_MUL") );
      }
    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrint( codeList );
    }
  else if( code == VPC1_SUB && binary->mOperand1->mType->isPointer() && binary->mType->isPointer() && binary->mOperand2->mType->isInt() ) {
    //Операция вычитания указателя с целым
    if( binary->mType->mBaseType->mSize > 1 ) {
      //Умножить на размер элемента
      gPushConst( prog, binary->mType->mBaseType->mSize, binary->mMark );
      prog->addCode( VPC1_MUL, binary->mMark );
      codePrint( QString("VPC1_MUL") );
      }
    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrint( codeList );
    }
  else
    errorInLine( QObject::tr("Error. This types not allowed for operation"), binary->mMark );

  if( keepValue ) {
    prog->addCode( VPC1_STORE, binary->mMark );
    codePrint( QString("VPC1_STORE") );
    }
  else {
    prog->addCode( VPC1_POP, binary->mMark );
    codePrint( QString("VPC1_POP") );
    }

  }







void SrVpuCompiler::gvvBinary(SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvBinary") );
    return;
    }
  if( binary->mOperand1 == 0 || binary->mOperand2 == 0 ) {
    binary->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Undefined operand binary to"), binary->mMark );
    return;
    }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  if( binary->mConst ) {
    if( keepValue )
      gPushConst( prog, binary->mConstInt, binary->mMark );
    }
  else {
    //Получить операнды в стеке и выполнить операцию
    gValue( prog, binary->mOperand1, keepValue, false );
    gValue( prog, binary->mOperand2, keepValue, false );

    //Проверить константу
    binary->checkConst();

    //Тип результата соответствует первому операнду
    binary->mType = binary->mOperand1->mType;

    //Разобраться с типами
    if( binary->mOperand1->mType->isInt() && binary->mOperand2->mType->isInt() ) {
      //Операнды целые

      //Выполнить операцию
      if( keepValue ) {
        prog->addCode( code, binary->mMark );
        codePrint( codeList );
        }
      }
    else if( code == VPC1_ADD && binary->mOperand1->mType->isPointer() && binary->mOperand2->mType->isInt() ) {
      //Операция сложения указателя с целым
      if( keepValue ) {
        if( binary->mType->mBaseType->mSize > 1 ) {
          //Умножить на размер элемента
          gPushConst( prog, binary->mType->mBaseType->mSize, binary->mMark );
          prog->addCode( VPC1_MUL, binary->mMark );
          codePrint( QString("VPC1_MUL") );
          }
        //Выполнить операцию
        prog->addCode( code, binary->mMark );
        codePrint( codeList );
        }
      }
    else if( code == VPC1_SUB && binary->mOperand1->mType->isPointer() && binary->mOperand2->mType->isInt() ) {
      //Операция вычитания указателя с целым
      if( keepValue ) {
        if( binary->mType->mBaseType->mSize > 1 ) {
          //Умножить на размер элемента
          gPushConst( prog, binary->mType->mBaseType->mSize, binary->mMark );
          prog->addCode( VPC1_MUL, binary->mMark );
          codePrint( QString("VPC1_MUL") );
          }
        //Выполнить операцию
        prog->addCode( code, binary->mMark );
        codePrint( codeList );
        }
      }
    else if( code == VPC1_SUB && binary->mOperand1->mType->isPointer() && binary->mOperand1->mType == binary->mOperand2->mType ) {
      //Операция вычитания двух указателей

      //Результат будет целым
      binary->mType = mTypeInt;

      //Выполнить операцию
      prog->addCode( code, binary->mMark );
      codePrint( codeList );

      if( binary->mOperand1->mType->mBaseType->mSize > 1 ) {
        //Делить расстояние на размер элемента
        gPushConst( prog, binary->mOperand1->mType->mBaseType->mSize, binary->mMark );
        prog->addCode( VPC1_DIV, binary->mMark );
        codePrint( QString("VPC1_DIV") );
        }

      }
    else
      errorInLine( QObject::tr("Error. This types not allowed for operation"), binary->mMark );

    }
  }






void SrVpuCompiler::gvvBinaryLong(SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvBinaryLong") );
    return;
    }
  if( binary->mOperand[0] == 0 || binary->mOperand[1] == 0 ) {
    binary->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Undefined operand binaryLong to"), binary->mMark );
    return;
    }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  if( binary->mConst ) {
    if( keepValue )
      gPushConst( prog, binary->mConstInt, binary->mMark );
    }
  else {
    //Получить операнды в стеке и выполнить операцию
    gValue( prog, binary->mOperand[0], keepValue, false );

    if( !binary->mOperand[0]->mType->isInt() )
      errorInLine( QObject::tr("Error. Operand must be int"), binary->mMark );

    binary->mConst = binary->mOperand[0]->mConst;
    if( binary->mConst ) binary->mConstInt = binary->mOperand[0]->mConstInt;

    //bool constPresent = false

    for( int i = 1; i < binary->mOperCount; i++ ) {
      gValue( prog, binary->mOperand[i], keepValue, false );
      if( binary->mConst && binary->mOperand[i]->mConst )
        binary->mConstInt = binary->constOperation( binary->mConstInt, binary->mOperand[i]->mConstInt );
      else
        binary->mConst = false;

      if( keepValue ) {
        prog->addCode( code, binary->mMark );
        codePrint( codeList );
        }
      }

    }
  }




void SrVpuCompiler::gvvLogAnd(SrProgramm *prog, SrValueLogAnd *binary, bool keepValue, bool address)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvLogAnd") );
    return;
    }
   binary->mType = mTypeInt;
   if( binary->mOperand[0] == 0 || binary->mOperand[1] == 0 ) {
     errorInLine( QObject::tr("Error. Undefined operand binaryLong to"), binary->mMark );
     return;
     }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  //Получить очередной операнд
  for( int i = 0; i < binary->mOperCount; i++ ) {
    //Получить очередной операнд
    if( binary->mOperand[i]->mConst )
      errorInLine( QObject::tr("Warning. Const in log expression"), binary->mMark );
    gValue( prog, binary->mOperand[i], true, false );
    //Проверка и переход на false
    if( keepValue ) {
      prog->addCodeParam24( VPC4_FALSE_JUMP_KEEP, binary->mExitLabel, binary->mMark );
      codePrint( QString("VPC4_FALSE_JUMP_KEEP LAB%1").arg(binary->mExitLabel) );
      }
    else {
      prog->addCodeParam24( VPC4_FALSE_JUMP, binary->mExitLabel, binary->mMark );
      codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(binary->mExitLabel) );
      }
    }
  //Если прошли все, то true
  if( keepValue ) {
    prog->addCode( VPC1_PUSH_1, binary->mMark );
    codePrint( QString("VPC1_PUSH_1") );
    }
  //Выходная метка
  binary->mExitLabel = prog->codeCount();
  codePrint( QString("LAB%1:").arg(binary->mExitLabel) );
  }




void SrVpuCompiler::gvvLogOr(SrProgramm *prog, SrValueLogOr *binary, bool keepValue, bool address)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvLogAnd") );
    return;
    }
   binary->mType = mTypeInt;
   if( binary->mOperand[0] == 0 || binary->mOperand[1] == 0 ) {
     errorInLine( QObject::tr("Error. Undefined operand binaryLong to"), binary->mMark );
     return;
     }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  //Получить очередной операнд
  for( int i = 0; i < binary->mOperCount; i++ ) {
    //Получить очередной операнд
    if( binary->mOperand[i]->mConst )
      errorInLine( QObject::tr("Warning. Const in log expression"), binary->mMark );
    gValue( prog, binary->mOperand[i], true, false );
    //Проверка и переход на true
    if( keepValue ) {
      prog->addCodeParam24( VPC4_TRUE_JUMP_KEEP, binary->mExitLabel, binary->mMark );
      codePrint( QString("VPC4_TRUE_JUMP_KEEP LAB%1").arg(binary->mExitLabel) );
      }
    else {
      prog->addCodeParam24( VPC4_TRUE_JUMP, binary->mExitLabel, binary->mMark );
      codePrint( QString("VPC4_TRUE_JUMP LAB%1").arg(binary->mExitLabel) );
      }
    }
  //Если прошли все, то true
  if( keepValue ) {
    prog->addCode( VPC1_PUSH_0, binary->mMark );
    codePrint( QString("VPC1_PUSH_0") );
    }
  //Выходная метка
  binary->mExitLabel = prog->codeCount();
  codePrint( QString("LAB%1:").arg(binary->mExitLabel) );
  }





void SrVpuCompiler::gvvComma(SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address)
  {
  if( binary == 0 ) {
    Error( QObject::tr("Error. gvvComma") );
    return;
    }
  binary->mType = mTypeInt;
  if( binary->mOperand[0] == 0 || binary->mOperand[1] == 0 ) {
    errorInLine( QObject::tr("Error. Undefined operand binaryLong to"), binary->mMark );
    return;
    }
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  //Получить очередной операнд
  for( int i = 0; i < binary->mOperCount-1; i++ )
    //Получить очередной операнд без сохранения результата
    gValue( prog, binary->mOperand[i], false, false );
  //Последний операнд может быть с сохранением
  gValue( prog, binary->mOperand[binary->mOperCount-1], keepValue, false );
  }




void SrVpuCompiler::gvvCondition(SrProgramm *prog, SrValueCondition *condition, bool keepValue, bool address)
  {
  if( condition == 0 ) {
    Error( QObject::tr("Error. gvvCondition") );
    return;
    }
  if( address ) {
    condition->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Need LValue"), condition->mMark );
    return;
    }
  if( condition->mCondition == 0 || condition->mTrue == 0 || condition->mFalse ) {
    condition->mType = mTypeInt;
    errorInLine( QObject::tr("Error in condition"), condition->mMark );
    return;
    }

  if( condition->mConst ) {
    if( keepValue )
      gPushConst( prog, condition->mConstInt, condition->mMark );
    }
  else {
    if( !condition->mCondition->isConst() ) {
      //Формировать условие
      gValue( prog, condition->mCondition, true, false );

      //Переход на ложное условие
      prog->addCodeParam24( VPC4_FALSE_JUMP, condition->mFalseAddress, condition->mMark );
      codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(condition->mFalseAddress) );
      }

    if( !condition->mCondition->isConst() || condition->mCondition->toConstInt() ) {
      //Ветка true
      gValue( prog, condition->mTrue, keepValue, false );
      condition->mType = condition->mTrue->mType;

      if( condition->mTrue->mConst && condition->mCondition->isConst() ) {
        condition->mConst = true;
        condition->mConstInt = condition->mTrue->mConstInt;
        }

      if( !condition->mCondition->isConst() ) {
        prog->addCodeParam24( VPC4_JUMP, condition->mExitAddress, condition->mMark );
        codePrint( QString("VPC4_JUMP LAB%1").arg(condition->mExitAddress) );
        }
      }

    condition->mFalseAddress = prog->codeCount();
    codePrint( QString("LAB%1:").arg(condition->mFalseAddress) );

    if( !condition->mCondition->isConst() || !condition->mCondition->toConstInt() ) {
      gValue( prog, condition->mFalse, keepValue, false );

      condition->mType = condition->mFalse->mType;

      if( condition->mFalse->mConst && condition->mCondition->isConst() ) {
        condition->mConst = true;
        condition->mConstInt = condition->mFalse->mConstInt;
        }

      condition->mExitAddress = prog->codeCount();
      codePrint( QString("LAB%1:").arg(condition->mExitAddress) );
      }

    //Проверим типы
    if( !condition->mCondition->isConst() && condition->mTrue->mType != condition->mFalse->mType )
      errorInLine( QObject::tr("Error. Types of condition must be same"), condition->mMark );

    }

  }





void SrVpuCompiler::gvvCall(SrProgramm *prog, SrValueCall *call, bool keepValue, bool address)
  {
  if( call == 0 )  {
    Error( QObject::tr("Error. gvvCall") );
    return;
    }
  if( address ) {
    call->mType = mTypeInt;
    errorInLine( QObject::tr("Error. Need LValue"), call->mMark );
    return;
    }
  if( call->mFunction == 0 ) {
    call->mType = mTypeInt;
    errorInLine( QObject::tr("Error in call"), call->mMark );
    return;
    }

  //Добыть адрес вызова
  gValue( prog, call->mFunction, true, false );

  //Тип вызываемой функции
  SrFunctionType *funType = call->mFunction->mType->toFunction();

  //Проверим, функцию ли мы вызываем
  if( funType == 0 )
    errorInLine( QObject::tr("Error. Call of nonfunction"), call->mMark );
  else {
    int resultCount = 0;
    //Если функция возвращает значение, то оставим под него место
    if( funType->mResult->isInt() || funType->mResult->isPointer() || funType->mResult->isObject() ) {
      //Возвращает целое или указатель или объект
      resultCount = 1;
      prog->addCode( VPC1_ALLOC_RESULT, call->mMark );
      codePrint( QString("VPC1_ALLOC_RESULT") );
      }
    else if( !funType->mResult->isVoid() ) {
      //Другие типы не допустимы
      errorInLine( QObject::tr("Error. Cant return this type"), call->mMark );
      return;
      }

    //Готовим параметры и проверяем их на соответствие формальным параметрам
    if( call->mParamCount < funType->mParamList.count() ) {
      errorInLine( QObject::tr("Error. Too low parameters in call"), call->mMark );
      return;
      }
    if( call->mParamCount > funType->mParamList.count() ) {
      errorInLine( QObject::tr("Error. Too mach parameters in call"), call->mMark );
      return;
      }

    //Маска объектов среди параметров
    int paramObjectMask = 0;
    int resultObjectMask = 0;
    //Формируем маску для результата
    if( funType->mResult->isObject() )
      resultObjectMask = 2;
    for( int i = 0; i < call->mParamCount; i++ ) {
      //Подготовить очередной параметр
      gValue( prog, call->mParam[i], true, false );
      //Формируем маску для параметров-объектов
      paramObjectMask <<= 1;
      resultObjectMask <<= 1;
      if( call->mParam[i]->mType->isObject() )
        paramObjectMask |= 1;
      //Проверим соответствие параметров
      if( !funType->mParamList.at(i).mType->isMatchParam(call->mParam[i]->mType, call->mParam[i]->mConst && call->mParam[i]->mConstInt == 0 ) )
        errorInLine( QObject::tr("Error. Type mismatch in parametr %1").arg(funType->mParamList.at(i).mName), call->mMark );
      }

    //Теперь вызов
    prog->addCodeParam8( VPC2_CALL, call->mParamCount, call->mMark );
    codePrint( QString("VPC2_CALL [%1]").arg(call->mParamCount) );

    //Теперь убираем все параметры
    if( keepValue ) {

      //Если среди параметров есть объекты, то очистить их
      if( paramObjectMask ) {
        if( paramObjectMask & 0xff000000 ) {
          //32 бита
          prog->addCodeParam32( VPC5_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC5_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else if( paramObjectMask & 0xff0000 ) {
          //24 бита
          prog->addCodeParam24( VPC4_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC4_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else if( paramObjectMask & 0xff00 ) {
          //16 бит
          prog->addCodeParam16( VPC3_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC3_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else {
          //8 бит
          prog->addCodeParam8( VPC2_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC2_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        }

      //Параметры и адрес вызова
      gStack( prog, call->mParamCount + 1, call->mMark );
      }
    else {
      paramObjectMask |= resultObjectMask;

      //Если среди параметров и результата есть объекты, то очистить их
      if( paramObjectMask ) {
        if( paramObjectMask & 0xff000000 ) {
          //32 бита
          prog->addCodeParam32( VPC5_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC5_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else if( paramObjectMask & 0xff0000 ) {
          //24 бита
          prog->addCodeParam24( VPC4_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC4_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else if( paramObjectMask & 0xff00 ) {
          //16 бит
          prog->addCodeParam16( VPC3_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC3_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        else {
          //8 бит
          prog->addCodeParam8( VPC2_OBJ_CLEAR_MAP, paramObjectMask, call->mMark );
          codePrint( QString("VPC2_OBJ_CLEAR_MAP %1").arg(paramObjectMask,0,2) );
          }
        }

      //Параметры, адрес вызова и результат, если есть
      gStack( prog, call->mParamCount + 1 + resultCount, call->mMark );
      }
    }
  }







