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
    Пико генератор байт-кода скриптового языка rc++

    Точка входа для построения и функции генерации
*/
#include "SrCompiler/SrVpuCompiler.h"
#include "SrVMachine/SrVmByteCode.h"
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
      prog->addCode( VBC1_WAIT, val->mMark );
      codePrintEoln( QString("VBC1_WAIT") );
      break;

    case svvExceptionFun :
      //Вызвать функцию возвращающую код исключения
      prog->addCode( VBC1_PUSH_THROW, val->mMark );
      codePrintEoln( QString("VBC1_PUSH_THROW") );
      break;

    case svvCatchFun :
      gValueCatchFun( prog, dynamic_cast<SrValueCatchFun*>(val) );
      break;

    case svvThrowFun :
      gValueThrowFun( prog, dynamic_cast<SrValueThrowFun*>(val) );
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
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_MUL, QString("VBC1_MUL") );
      break;

    case svvMulStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_MUL, QString("VBC1_MUL") );
      break;

    case svvDiv :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_DIV, QString("VBC1_DIV") );
      break;

    case svvDivStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_DIV, QString("VBC1_DIV") );
      break;

    case svvMod :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_MOD, QString("VBC1_MOD") );
      break;

    case svvModStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_MOD, QString("VBC1_MOD") );
      break;

    case svvAdd :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_ADD, QString("VBC1_ADD") );
      break;

    case svvAddStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_ADD, QString("VBC1_ADD") );
      break;

    case svvSub :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_SUB, QString("VBC1_SUB") );
      break;

    case svvSubStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_SUB, QString("VBC1_SUB") );
      break;

    case svvLShift :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_LSHIFT, QString("VBC1_LSHIFT") );
      break;

    case svvLShiftStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_LSHIFT, QString("VBC1_LSHIFT") );
      break;

    case svvRShift :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_RSHIFT, QString("VBC1_RSHIFT") );
      break;

    case svvRShiftStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_RSHIFT, QString("VBC1_RSHIFT") );
      break;

    case svvOrStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_OR, QString("VBC1_OR") );
      break;

    case svvAndStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_AND, QString("VBC1_AND") );
      break;

    case svvXorStore :
      gvvBinaryStore( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_XOR, QString("VBC1_XOR") );
      break;

    case svvEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_EQU, QString("VBC1_EQU") );
      break;

    case svvNotEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_NOT_EQU, QString("VBC1_NOT_EQU") );
      break;

    case svvLessEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_LESS_EQU, QString("VBC1_LESS_EQU") );
      break;

    case svvLess :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_LESS, QString("VBC1_LESS") );
      break;

    case svvGreat :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_GREAT, QString("VBC1_GREAT") );
      break;

    case svvGreatEqu :
      gvvBinary( prog, dynamic_cast<SrValueBinary*>(val), keepValue, address, VBC1_GREAT_EQU, QString("VBC1_GREAT_EQU") );
      break;

    case svvLogAnd :
      gvvLogAnd( prog, dynamic_cast<SrValueLogAnd*>(val), keepValue, address );
      break;

    case svvLogOr :
      gvvLogOr( prog, dynamic_cast<SrValueLogOr*>(val), keepValue, address );
      break;

    case svvOr :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VBC1_OR, QString("VBC1_OR") );
      break;

    case svvAnd :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VBC1_AND, QString("VBC1_AND") );
      break;

    case svvXor :
      gvvBinaryLong( prog, dynamic_cast<SrValueBinaryLong*>(val), keepValue, address, VBC1_XOR, QString("VBC1_XOR") );
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
  Q_ASSERT( var != nullptr );

  Q_ASSERT( var->mVariable != nullptr );

  if( var->mConst ) {
    if( keepValue ) {
      gPushConst( prog, var->mConstInt, var->mMark );
      codePrintEoln( QString("//%1").arg(var->mVariable->mName) );
      }
    }
  else {

    //Проверить на массив
    if( (address || var->mVariable->mType->isArray()) && keepValue ) {
      switch ( var->mVariable->mSort ) {
        case tsrGlobal :
          //Глобальная переменная
          gPushConst( prog, var->mVariable->mAddress + var->mAddonAddress, var->mMark );
          if( var->mAddonAddress )
            codePrintEoln( QString(" [%1.%2] //%3").arg(var->mVariable->mAddress + var->mAddonAddress)
                           .arg(var->mAddonAddress).arg(var->mVariable->mName) );
          else
            codePrintEoln( QString("//%1").arg(var->mVariable->mName) );
          var->mConst = true;
          var->mConstInt = var->mVariable->mAddress;
          break;

        case tsrLocal :
          //Локальная переменная
          prog->addCodeParam8( VBC2_PUSH_B_OFFSET, var->mVariable->mAddress + var->mAddonAddress, var->mMark );
          if( var->mAddonAddress )
            codePrintEoln( QString("VBC2_PUSH_B_OFFSET %1 [%2.%3] //%4").arg(var->mVariable->mAddress + var->mAddonAddress)
                           .arg( var->mVariable->mAddress ).arg( var->mAddonAddress ).arg(var->mVariable->mName) );
          else
            codePrintEoln( QString("VBC2_PUSH_B_OFFSET %1 //%2").arg(var->mVariable->mAddress).arg(var->mVariable->mName) );
          break;

        case tsrMember :
          errorInLine( QObject::tr("Internal error 1."), var->mMark );
          break;
        }
      }
    else if( keepValue ) {
      switch ( var->mVariable->mSort ) {
        case tsrGlobal :
          //Глобальная переменная
          prog->addCodeParam24( VBC4_PUSH_GLOBAL, var->mVariable->mAddress + var->mAddonAddress, var->mMark );
          if( var->mAddonAddress )
            codePrintEoln( QString("VBC4_PUSH_GLOBAL %1 [%2.%3] //%4").arg(var->mVariable->mAddress + var->mAddonAddress)
                           .arg( var->mVariable->mAddress ).arg( var->mAddonAddress ).arg(var->mVariable->mName) );
          else
            codePrintEoln( QString("VBC4_PUSH_GLOBAL %1 //%2").arg(var->mVariable->mAddress).arg(var->mVariable->mName) );

          break;

        case tsrLocal :
          //Локальная переменная
          prog->addCodeParam8( VBC2_PUSH_LOCAL, var->mVariable->mAddress + var->mAddonAddress, var->mMark );
          if( var->mAddonAddress )
            codePrintEoln( QString("VBC2_PUSH_LOCAL %1 [%2.%3] //%4").arg(var->mVariable->mAddress + var->mAddonAddress)
                           .arg( var->mVariable->mAddress ).arg( var->mAddonAddress ).arg(var->mVariable->mName) );
          else
            codePrintEoln( QString("VBC2_PUSH_LOCAL %1 //%2").arg(var->mVariable->mAddress).arg(var->mVariable->mName) );
          break;

        case tsrMember :
          errorInLine( QObject::tr("Internal error 2."), var->mMark );
          break;
        }
      }

    //Разобраться с типами
    if( address ) {
      //В стеке указатель
      //Для массива - оставляем тип массива
      if( var->mVariable->mType->isArray() )
        var->setType( var->mVariable->mType );
      else
        //В остальных случаях - формируем указатель
        var->setType( mTypeList.getTypePointer(var->mVariable->mType) );
      }
    else
      //Тип переменной
      var->setType( var->mVariable->mType );

    }

  }








//Загрузить адрес функции
void SrVpuCompiler::gvvFunction(SrProgramm *prog, SrValueFunction *fun, bool keepValue, bool address)
  {
  Q_ASSERT( fun != nullptr );

  if( !address ) {
    errorInLine( QObject::tr("Error. Need LValue"), fun->mMark );
    return;
    }

  Q_ASSERT( fun->mFunction != nullptr );


  if( keepValue ) {

    if( fun->mFunction->mImportIndex ) {
      //Импортная функция
      prog->addCodeParam32( VBC5_PUSH_CONST, fun->mFunction->mImportIndex | 0x80000000, fun->mMark );
      codePrintEoln( QString("VBC5_PUSH_CONST %2 | 0x80000000 //import func index %1").arg(fun->mFunction->mName).arg(fun->mFunction->mImportIndex) );
      fun->mConst = true;
      fun->mConstInt = fun->mFunction->mImportIndex | 0x80000000;
      }
    else {
      //Глобальная функция
      gPushConst( prog, fun->mFunction->mAddress, fun->mMark );
      codePrintEoln( QString(" //func address %1").arg(fun->mFunction->mName) );
      fun->mConst = true;
      fun->mConstInt = fun->mFunction->mAddress;
      }
    }
  }






void SrVpuCompiler::gvvConstInt(SrProgramm *prog, SrValueConstInt *cnst, bool keepValue, bool address)
  {
  Q_ASSERT( cnst != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), cnst->mMark );
    return;
    }
  if( keepValue ) {
    //Загружаем константное значение
    gPushConst( prog, cnst->mConstInt, cnst->mMark );
    codePrintEoln( QString() );
    }
  }






void SrVpuCompiler::gvvConstString(SrProgramm *prog, SrValueConstString *cnst, bool keepValue, bool address)
  {
  Q_ASSERT( cnst != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), cnst->mMark );
    return;
    }

  if( keepValue ) {
    //Загружаем константное значение
    gPushConst( prog, cnst->mIndex, cnst->mMark );
    codePrintEoln( QString("// \"%1\"").arg(mStringTable.at(cnst->mIndex)) );
    }
  }





void SrVpuCompiler::gvvPointer(SrProgramm *prog, SrValuePointer *ptr, bool keepValue, bool address)
  {
  Q_ASSERT( ptr != nullptr );

  Q_ASSERT( ptr->mOperand != nullptr );

  gValue( prog, ptr->mOperand, keepValue, false );

  //Разобраться с типами
  ptr->setType(ptr->mOperand->getType());
  if( !address )
    ptr->setType( ptr->getType()->mBaseType );

  //Убедиться, что имеем дело с указателем
  if( !ptr->mOperand->getType()->isPointer() )
    errorInLine( QObject::tr("Error. Pointer not allowed in this context"), ptr->mMark );

  if( keepValue ) {
    if( !address && !ptr->getType()->isStruct() ) {
      //Нужно само значение по указателю
      gLoad( prog, ptr );
      codePrintEoln( QString("// *pointer") );
      }
    }
  }






void SrVpuCompiler::gvvMemberVariable(SrProgramm *prog, SrValueMemberVariable *var, bool keepValue, bool address)
  {
  Q_ASSERT( var != nullptr );

  Q_ASSERT( var->mVariable != nullptr );

  //Проверить на массив
  bool isArray = var->mVariable->mType->isArray() && keepValue;

  //Если структура имеет константный адрес, то сразу все вычисляем
  if( var->mStruct->isConst() ) {
    //Глобальная переменная
    if( keepValue ) {
      int addr = var->mStruct->mConstInt + var->mVariable->mAddress + var->mAddonAddress;
      if( address || isArray ) {
        //Нам нужен адрес переменной
        gPushConst( prog, addr, var->mMark );
        codePrintEoln( QString("//address .%1").arg(var->mVariable->mName) );
        }
      else {
        //Нам нужно значение
        prog->addCodeParam24( VBC4_PUSH_GLOBAL, addr, var->mMark );
        codePrintEoln( QString("VBC4_PUSH_GLOBAL %1 [%2.%3.%4] //.%5").arg(addr)
                       .arg(var->mStruct->mConstInt).arg(var->mVariable->mAddress).arg(var->mAddonAddress).arg(var->mVariable->mName) );
        }
      }
    }
  else {
    //Получить значение указателя
    SrValueVariable *structVar = dynamic_cast<SrValueVariable*>(var->mStruct);
    if( structVar != nullptr ) {
      //Если есть непосредственный доступ к переменной, то выполнить
      structVar->mAddonAddress = var->mVariable->mAddress + var->mAddonAddress;
      gValue( prog, var->mStruct, keepValue, address || isArray );
      }
    else {
      gValue( prog, var->mStruct, keepValue, true );
      if( keepValue ) {
        //Суммируем с адресом члена
        int addr = var->mVariable->mAddress + var->mAddonAddress;
        switch( addr ) {
          case 0 :
            break;
          case 1 :
            prog->addCode( VBC1_ADD_1, var->mMark );
            codePrintEoln( QString("VBC1_ADD_1 //offset and address .%1").arg(var->mVariable->mName) );
            break;
          case 2 :
            prog->addCode( VBC1_ADD_2, var->mMark );
            codePrintEoln( QString("VBC1_ADD_2 //offset and address .%1").arg(var->mVariable->mName) );
            break;
          case 3 :
            prog->addCode( VBC1_ADD_3, var->mMark );
            codePrintEoln( QString("VBC1_ADD_3 //offset and address .%1").arg(var->mVariable->mName) );
            break;
          case 4 :
            prog->addCode( VBC1_ADD_4, var->mMark );
            codePrintEoln( QString("VBC1_ADD_4 //offset and address .%1").arg(var->mVariable->mName) );
            break;
          default :
            if( addr < 127 ) {
              prog->addCodeParam8( VBC2_ADD_CONST, addr, var->mMark );
              codePrint( QString("VBC2_ADD_CONST %1 //offset and address /%2").arg(addr).arg(var->mVariable->mName) );
              }
            else {
              gPushConst( prog, addr, var->mMark );
              codePrintEoln( QString("//offset .%1").arg(var->mVariable->mName) );
              prog->addCode( VBC1_ADD, var->mMark );
              codePrintEoln( QString("VBC1_ADD //address .%1").arg(var->mVariable->mName) );
              }
            break;
          }
        if( !address ) {
          //Нужно само значение по указателю
          gLoad( prog, var );
          codePrintEoln( QString("//value .%1").arg(var->mVariable->mName) );
          }
        }
      }
    }

  //Разобраться с типами
  if( address )
    //Нужен адрес
    var->setType( mTypeList.getTypePointer(var->mVariable->mType) );
  else
    //Нужно значение
    var->setType( var->mVariable->mType );
  }






void SrVpuCompiler::gvvAddress(SrProgramm *prog, SrValueAddress *addr, bool keepValue, bool address)
  {
  Q_ASSERT( addr != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), addr->mMark );
    return;
    }

  if( addr->mConst ) {
    if( keepValue ) {
      gPushConst( prog, addr->mConstInt, addr->mMark );
      codePrintEoln( QString("//address") );
      }
    }
  else {
    gValue( prog, addr->mOperand, keepValue, true );
    addr->setType( addr->mOperand->getType() );
    addr->mConst = addr->mOperand->mConst;
    if( addr->mConst )
      addr->mConstInt = addr->mOperand->mConstInt;

    //Если поступил не указатель, то взятие адреса не удалось
    if( !addr->getType()->isPointer() )
      errorInLine( QObject::tr("Error. Cant take address. Undefined context."), addr->mMark );
    }
  }







void SrVpuCompiler::gvvPredInc(SrProgramm *prog, SrValuePredInc *inc, bool keepValue, bool address)
  {
  Q_ASSERT( inc != nullptr );

  Q_ASSERT( inc->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), inc->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, inc->mOperand, true, true );

  if( inc->mOperand->getType()->isPointer() )
    inc->setType( inc->mOperand->getType()->mBaseType );
  else {
    ErrorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark.mFile, inc->mMark.mLine );
    }


  if( inc->getType()->isInt() ) {
    //Для целых чисел инкремент на 1
    if( keepValue ) {
      prog->addCode( VBC1_PRED_INC, inc->mMark );
      codePrintEoln( QString("VBC1_PRED_INC") );
      }
    else {
      prog->addCode( VBC1_INC, inc->mMark );
      codePrintEoln( QString("VBC1_INC") );
      }
    }
  else if( inc->getType()->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VBC4_PRED_INC, inc->getType()->mBaseType->mSize, inc->mMark );
      codePrintEoln( QString("VBC4_PRED_INC %1").arg(inc->getType()->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VBC4_INC, inc->getType()->mBaseType->mSize, inc->mMark );
      codePrintEoln( QString("VBC4_INC %1").arg(inc->getType()->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred increment not allowed with this type"), inc->mMark );

  }






void SrVpuCompiler::gvvPredDec(SrProgramm *prog, SrValuePredDec *dec, bool keepValue, bool address)
  {
  Q_ASSERT( dec != nullptr );

  Q_ASSERT( dec->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), dec->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, dec->mOperand, true, true );

  if( dec->mOperand->getType()->isPointer() )
    dec->setType( dec->mOperand->getType()->mBaseType );
  else
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );


  if( dec->getType()->isInt() ) {
    //Для целых чисел декремент на 1
    if( keepValue ) {
      prog->addCode( VBC1_PRED_DEC, dec->mMark );
      codePrintEoln( QString("VBC1_PRED_DEC") );
      }
    else {
      prog->addCode( VBC1_DEC, dec->mMark );
      codePrintEoln( QString("VBC1_DEC") );
      }
    }
  else if( dec->getType()->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VBC4_PRED_DEC, dec->getType()->mBaseType->mSize, dec->mMark );
      codePrintEoln( QString("VBC4_PRED_DEC %1").arg(dec->getType()->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VBC4_DEC, dec->getType()->mBaseType->mSize, dec->mMark );
      codePrintEoln( QString("VBC4_DEC %1").arg(dec->getType()->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred decrement not allowed with this type"), dec->mMark );

  }




void SrVpuCompiler::gvvPostInc(SrProgramm *prog, SrValuePostInc *inc, bool keepValue, bool address)
  {
  Q_ASSERT( inc != nullptr );

  Q_ASSERT( inc->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), inc->mMark );
    return;
    }

  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, inc->mOperand, true, true );

  if( inc->mOperand->getType()->isPointer() )
    inc->setType( inc->mOperand->getType()->mBaseType );
  else
    errorInLine( QObject::tr("Error. Undefined increment to"), inc->mMark );

  if( inc->getType()->isInt() ) {
    //Для целых чисел инкремент на 1
    if( keepValue ) {
      prog->addCode( VBC1_POST_INC, inc->mMark );
      codePrintEoln( QString("VBC1_POST_INC") );
      }
    else {
      prog->addCode( VBC1_INC, inc->mMark );
      codePrintEoln( QString("VBC1_INC") );
      }
    }
  else if( inc->getType()->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VBC4_POST_INC, inc->getType()->mBaseType->mSize, inc->mMark );
      codePrintEoln( QString("VBC4_POST_INC %1").arg(inc->getType()->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VBC4_INC, inc->getType()->mBaseType->mSize, inc->mMark );
      codePrintEoln( QString("VBC4_INC %1").arg(inc->getType()->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Post increment not allowed with this type"), inc->mMark );
  }




void SrVpuCompiler::gvvPostDec(SrProgramm *prog, SrValuePostDec *dec, bool keepValue, bool address)
  {
  Q_ASSERT( dec != nullptr );

  Q_ASSERT( dec->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), dec->mMark );
    return;
    }


  //Получить адрес, по которому нужно выполнить инкремент
  gValue( prog, dec->mOperand, true, true );

  if( dec->mOperand->getType()->isPointer() )
    dec->setType( dec->mOperand->getType()->mBaseType );
  else
    errorInLine( QObject::tr("Error. Undefined decrement to"), dec->mMark );


  if( dec->getType()->isInt() ) {
    //Для целых чисел декремент на 1
    if( keepValue ) {
      prog->addCode( VBC1_POST_DEC, dec->mMark );
      codePrintEoln( QString("VBC1_POST_DEC") );
      }
    else {
      prog->addCode( VBC1_DEC, dec->mMark );
      codePrintEoln( QString("VBC1_DEC") );
      }
    }
  else if( dec->getType()->isPointer() ) {
    //Для указателей инкремент на размер объекта
    if( keepValue ) {
      prog->addCodeParam24( VBC4_POST_DEC, dec->getType()->mBaseType->mSize, dec->mMark );
      codePrintEoln( QString("VBC4_POST_DEC %1").arg(dec->getType()->mBaseType->mSize) );
      }
    else {
      prog->addCodeParam24( VBC4_DEC, dec->getType()->mBaseType->mSize, dec->mMark );
      codePrintEoln( QString("VBC4_DEC %1").arg(dec->getType()->mBaseType->mSize) );
      }
    }
  else
    errorInLine( QObject::tr("Error. Pred decrement not allowed with this type"), dec->mMark );

  }






void SrVpuCompiler::gvvNot(SrProgramm *prog, SrValueBitNot *vnot, bool keepValue, bool address)
  {
  Q_ASSERT( vnot != nullptr );

  Q_ASSERT( vnot->mOperand != nullptr );
  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), vnot->mMark );
    return;
    }

  if( vnot->mConst ) {
    if( keepValue ) {
      gPushConst( prog, vnot->mConstInt, vnot->mMark );
      codePrintEoln( QString("// ~") );
      }
    }
  else {
    //Получить значение
    gValue( prog, vnot->mOperand, keepValue, false );

    //Разобраться с константой
    vnot->checkConst();

    //Если операнд не целого типа - то ошибка
    if( !vnot->mOperand->getType()->isInt() )
      errorInLine( QObject::tr("Error. Allowed only with int"), vnot->mMark );

    if( keepValue ) {
      prog->addCode( VBC1_NOT, vnot->mMark );
      codePrintEoln( QString("VBC1_NOT") );
      }
    }
  }





void SrVpuCompiler::gvvLogNot(SrProgramm *prog, SrValueLogNot *logNot, bool keepValue, bool address)
  {
  Q_ASSERT( logNot != nullptr );

  Q_ASSERT( logNot->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), logNot->mMark );
    return;
    }

  if( logNot->mConst ) {
    if( keepValue ) {
      gPushConst( prog, logNot->mConstInt, logNot->mMark );
      codePrintEoln( QString("// !") );
      }
    }
  else {
    //Получить значение
    gValue( prog, logNot->mOperand, keepValue, false );

    //Разобраться с константой
    logNot->checkConst();

    if( !(logNot->mOperand->getType()->isInt() || logNot->mOperand->getType()->isPointer()) ) {
      errorInLine( QObject::tr("Error. LogNot allowed only with int and pointer"), logNot->mMark );
      }

    if( keepValue ) {
      prog->addCode( VBC1_LNOT, logNot->mMark );
      codePrintEoln( QString("VBC1_LNOT") );
      }
    }
  }




void SrVpuCompiler::gvvNeg(SrProgramm *prog, SrValueNeg *neg, bool keepValue, bool address)
  {
  Q_ASSERT( neg != nullptr );

  Q_ASSERT( neg->mOperand != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), neg->mMark );
    return;
    }

  if( neg->mConst ) {
    if( keepValue ) {
      gPushConst( prog, neg->mConstInt, neg->mMark );
      codePrintEoln( QString("// -") );
      }
    }
  else {
    //Получить значение
    gValue( prog, neg->mOperand, keepValue, false );

    //Разобраться с константой
    neg->checkConst();

    //Если операнд не целого типа - то ошибка
    if( !neg->mOperand->getType()->isInt() )
      errorInLine( QObject::tr("Error. NEG allowed only with int"), neg->mMark );

    if( keepValue ) {
      prog->addCode( VBC1_NEG, neg->mMark );
      codePrintEoln( QString("VBC1_NEG") );
      }
    }
  }




void SrVpuCompiler::gvvStore(SrProgramm *prog, SrValueStore *store, bool keepValue, bool address)
  {
  Q_ASSERT( store != nullptr );

  Q_ASSERT( store->mOperand1 != nullptr && store->mOperand2 != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), store->mMark );
    return;
    }

   //Для той, куда сохраняем берем адрес
  gValue( prog, store->mOperand1, true, true );
  //Теперь значение
  gValue( prog, store->mOperand2, true, false );

  //Тип результата соответствует типу операнда 2
  store->setType( store->mOperand2->getType() );

  //Проверить возможность сохранения
  //Сохранять можем только по указателю
  if( !store->mOperand1->getType()->isPointer() ) {
    errorInLine( QObject::tr("Error. Need lvalue, but %1 is not").arg(store->mOperand1->getType()->mName), store->mMark );
    return;
    }
  if( !store->mOperand1->getType()->mBaseType->canAssign( store->mOperand2->getType() ) ) {
    errorInLine( QObject::tr("Error. Can't store %1 to %2").arg(store->mOperand2->getType()->mName).arg(store->mOperand1->getType()->mBaseType->mName), store->mMark );
    return;
    }
  if( keepValue ) {
    //Сохранение значения
    prog->addCode( VBC1_STORE, store->mMark );
    codePrintEoln( QString("VBC1_STORE") );
    }
  else {
    //Сохранение значения
    prog->addCode( VBC1_POP, store->mMark );
    codePrintEoln( QString("VBC1_POP") );
    }
  }





void SrVpuCompiler::gvvArrayCell(SrProgramm *prog, SrValueArrayCell *array, bool keepValue, bool address)
  {
  Q_ASSERT( array != nullptr );

  Q_ASSERT( array->mOperand1 != nullptr && array->mOperand2 != nullptr );

  if( array->mConst ) {
    if( keepValue ) {
      gPushConst( prog, array->mConstInt, array->mMark );
      codePrintEoln( QString("//array cell offset and address") );
      if( !address ) {
        gLoad( prog, array );
        codePrintEoln( QString("//array cell value") );
        }
      }
    }
  else if( array->mOperand2->isConst() ) {
    //Получить адрес массива
    gValue( prog, array->mOperand1, keepValue, false );
    //Загрузить константное смещение и сложить
    int offset = array->mOperand2->mConstInt * array->mOperand1->getType()->mBaseType->mSize;
    switch( offset ) {
      case 0 :
        break;
      case 1 :
        prog->addCode( VBC1_ADD_1, array->mMark );
        codePrintEoln( QString("VBC1_ADD_1 //array cell [%1] offset and address").arg(array->mOperand2->mConstInt) );
        break;
      case 2 :
        prog->addCode( VBC1_ADD_2, array->mMark );
        codePrintEoln( QString("VBC1_ADD_2 //array cell [%1] offset and address").arg(array->mOperand2->mConstInt) );
        break;
      case 3 :
        prog->addCode( VBC1_ADD_3, array->mMark );
        codePrintEoln( QString("VBC1_ADD_3 //array cell [%1] offset and address").arg(array->mOperand2->mConstInt) );
        break;
      case 4 :
        prog->addCode( VBC1_ADD_4, array->mMark );
        codePrintEoln( QString("VBC1_ADD_4 //array cell [%1] offset and address").arg(array->mOperand2->mConstInt) );
        break;
      default :
        if( offset < 127 ) {
          prog->addCodeParam8( VBC2_ADD_CONST, offset, array->mMark );
          codePrintEoln( QString("VBC2_ADD_CONST %1 //array cell [%2] offset and address").arg(offset).arg(array->mOperand2->mConstInt) );
          }
        else {
          gPushConst( prog, offset, array->mMark );
          codePrintEoln( QString("//array cell [%1] offset").arg(array->mOperand2->mConstInt) );
          prog->addCode( VBC1_ADD, array->mMark );
          codePrintEoln( QString("VBC1_ADD //array cell [%1] address").arg(array->mOperand2->mConstInt) );
          }
        break;
      }
    if( !address ) {
      gLoad( prog, array );
      codePrintEoln( QString("//array cell [%1] value").arg(array->mOperand2->mConstInt) );
      }
    }
  else {
    //Получить адрес массива
    gValue( prog, array->mOperand1, keepValue, false );
    codePrintEoln( QString("//array address") );
    //Ячейку массива можем получать только у массива и указателя
    if( !(array->mOperand1->getType()->isArray() || array->mOperand1->getType()->isPointer()) ) {
      errorInLine( QObject::tr("Error. Index allowed only for arrays and pointers"), array->mMark );
      return;
      }

    //Получить индекс
    gValue( prog, array->mOperand2, keepValue, false );
    codePrintEoln( QString("//array index") );
    if( !array->mOperand2->getType()->isInt() ) {
      errorInLine( QObject::tr("Error. Index must be integer"), array->mMark );
      return;
      }

    //Разобраться с константой
    if( address && array->mOperand1->mConst && array->mOperand2->mConst ) {
      //Оба операнда - константы
      array->mConst = true;
      array->mConstInt = array->mOperand1->mConstInt + array->mOperand2->mConstInt * array->mOperand1->getType()->mBaseType->mSize;
      }

    if( keepValue ) {
      if( array->mOperand1->getType()->mBaseType->mSize > 1 ) {
        //Умножить на размер ячейки
        gPushConst( prog, array->mOperand1->getType()->mBaseType->mSize, array->mMark );
        codePrintEoln( QString("//array cell size") );
        prog->addCode( VBC1_MUL, array->mMark );
        codePrintEoln( QString("VBC1_MUL //array cell offset") );
        }
      prog->addCode( VBC1_ADD, array->mMark );
      codePrintEoln( QString("VBC1_ADD //array cell address") );
      if( !address ) {
        gLoad( prog, array );
        codePrintEoln( QString("//array cell value") );
        }
      }
    }

  //Разобраться с типом
  if( address )
    array->setType( mTypeList.getTypePointer( array->mOperand1->getType()->mBaseType ) );
  else
    array->setType( array->mOperand1->getType()->mBaseType );
  }




void SrVpuCompiler::gvvBinaryStore(SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVmCode code, const QString codeList)
  {
  Q_ASSERT( binary != nullptr );

  Q_ASSERT( binary->mOperand1 != nullptr && binary->mOperand2 != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }

  //Получить операнды в стеке и выполнить операцию
  gValue( prog, binary->mOperand1, true, true );
  //Тип результата соответсвует тому, куда указывает операнд 1
  if( !binary->mOperand1->getType()->isPointer() ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }
  binary->setType( binary->mOperand1->getType()->mBaseType );
  //Сдублировать адрес
  prog->addCode( VBC1_PUSH_TOS, binary->mMark );
  codePrintEoln( QString("VBC1_PUSH_TOS") );

  //Получить значение для операции
  gLoad( prog, binary );
  codePrintEoln( QString() );

  //Второй операнд
  gValue( prog, binary->mOperand2, true, false );

  //Проверить типы операндов
  if( binary->getType()->isInt() && binary->mOperand2->getType()->isInt() ) {
    //Операнды целые

    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrintEoln( codeList );
    }
  else if( code == VBC1_ADD && binary->getType()->isPointer() && binary->mOperand2->getType()->isInt() ) {
    //Операция сложения указателя с целым
    if( binary->getType()->mBaseType->mSize > 1 ) {
      //Умножить на размер элемента
      gPushConst( prog, binary->getType()->mBaseType->mSize, binary->mMark );
      codePrintEoln( QString("//element size") );
      prog->addCode( VBC1_MUL, binary->mMark );
      codePrintEoln( QString("VBC1_MUL //pointer offset") );
      }
    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrintEoln( codeList );
    }
  else if( code == VBC1_SUB && binary->getType()->isPointer() && binary->mOperand2->getType()->isInt() ) {
    //Операция вычитания указателя с целым
    if( binary->getType()->mBaseType->mSize > 1 ) {
      //Умножить на размер элемента
      gPushConst( prog, binary->getType()->mBaseType->mSize, binary->mMark );
      codePrintEoln( QString("//element size") );
      prog->addCode( VBC1_MUL, binary->mMark );
      codePrintEoln( QString("VBC1_MUL //pointer offset") );
      }
    //Выполнить операцию
    prog->addCode( code, binary->mMark );
    codePrintEoln( codeList );
    }
  else
    errorInLine( QObject::tr("Error. This types not allowed for operation"), binary->mMark );

  if( keepValue ) {
    prog->addCode( VBC1_STORE, binary->mMark );
    codePrintEoln( QString("VBC1_STORE") );
    }
  else {
    prog->addCode( VBC1_POP, binary->mMark );
    codePrintEoln( QString("VBC1_POP") );
    }

  }







void SrVpuCompiler::gvvBinary(SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVmCode code, const QString codeList)
  {
  Q_ASSERT( binary != nullptr );

  Q_ASSERT( binary->mOperand1 != nullptr && binary->mOperand2 != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }

  if( binary->mConst ) {
    if( keepValue ) {
      gPushConst( prog, binary->mConstInt, binary->mMark );
      codePrintEoln( QString() );
      }
    }
  else {
    //Получить операнды в стеке и выполнить операцию
    gValue( prog, binary->mOperand1, keepValue, false );
    gValue( prog, binary->mOperand2, keepValue, false );

    //Проверить константу
    binary->checkConst();

    //Тип результата соответствует первому операнду
    binary->setType( binary->mOperand1->getType() );

    //Разобраться с типами
    if( binary->mOperand1->getType()->isInt() && binary->mOperand2->getType()->isInt() ) {
      //Операнды целые

      //Выполнить операцию
      if( keepValue ) {
        prog->addCode( code, binary->mMark );
        codePrintEoln( codeList );
        }
      }
    else if( code == VBC1_ADD && binary->mOperand1->getType()->isPointer() && binary->mOperand2->getType()->isInt() ) {
      //Операция сложения указателя с целым
      if( keepValue ) {
        if( binary->getType()->mBaseType->mSize > 1 ) {
          //Умножить на размер элемента
          gPushConst( prog, binary->getType()->mBaseType->mSize, binary->mMark );
          codePrintEoln( QString("//element size") );
          prog->addCode( VBC1_MUL, binary->mMark );
          codePrintEoln( QString("VBC1_MUL //pointer offset") );
          }
        //Выполнить операцию
        prog->addCode( code, binary->mMark );
        codePrintEoln( codeList );
        }
      }
    else if( code == VBC1_SUB && binary->mOperand1->getType()->isPointer() && binary->mOperand2->getType()->isInt() ) {
      //Операция вычитания указателя с целым
      if( keepValue ) {
        if( binary->getType()->mBaseType->mSize > 1 ) {
          //Умножить на размер элемента
          gPushConst( prog, binary->getType()->mBaseType->mSize, binary->mMark );
          codePrintEoln( QString("//element size") );
          prog->addCode( VBC1_MUL, binary->mMark );
          codePrintEoln( QString("VBC1_MUL //pointer offset") );
          }
        //Выполнить операцию
        prog->addCode( code, binary->mMark );
        codePrintEoln( codeList );
        }
      }
    else if( code == VBC1_SUB && binary->mOperand1->getType()->isPointer() && binary->mOperand1->getType() == binary->mOperand2->getType() ) {
      //Операция вычитания двух указателей

      //Результат будет целым
      binary->setType( mTypeInt );

      //Выполнить операцию
      prog->addCode( code, binary->mMark );
      codePrintEoln( codeList );

      if( binary->mOperand1->getType()->mBaseType->mSize > 1 ) {
        //Делить расстояние на размер элемента
        gPushConst( prog, binary->mOperand1->getType()->mBaseType->mSize, binary->mMark );
        codePrintEoln( QString("//element size") );
        prog->addCode( VBC1_DIV, binary->mMark );
        codePrintEoln( QString("VBC1_DIV //element count") );
        }

      }
    else
      errorInLine( QObject::tr("Error. This types not allowed for operation"), binary->mMark );

    }
  }






void SrVpuCompiler::gvvBinaryLong(SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address, SrVmCode code, const QString codeList)
  {
  Q_ASSERT( binary != nullptr );

  Q_ASSERT( binary->mOperand[0] != nullptr && binary->mOperand[1] != nullptr && binary->mOperCount > 1 );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), binary->mMark );
    return;
    }

  if( binary->mConst ) {
    if( keepValue ) {
      gPushConst( prog, binary->mConstInt, binary->mMark );
      codePrintEoln( QString() );
      }
    }
  else {
    //Получить операнды в стеке и выполнить операцию
    gValue( prog, binary->mOperand[0], keepValue, false );

    if( !binary->mOperand[0]->getType()->isInt() )
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
        codePrintEoln( codeList );
        }
      }

    }
  }




void SrVpuCompiler::gvvLogAnd(SrProgramm *prog, SrValueLogAnd *binary, bool keepValue, bool address)
  {
  Q_ASSERT( binary != nullptr );

  Q_ASSERT( binary->mOperand[0] != nullptr && binary->mOperand[1] != nullptr && binary->mOperCount > 1 );

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
      prog->addCodeParam24( VBC4_FALSE_JUMP_KEEP, binary->mExitLabel, binary->mMark );
      codePrintEoln( QString("VBC4_FALSE_JUMP_KEEP LAB%1").arg(binary->mExitLabel) );
      }
    else {
      prog->addCodeParam24( VBC4_FALSE_JUMP, binary->mExitLabel, binary->mMark );
      codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1").arg(binary->mExitLabel) );
      }
    }
  //Если прошли все, то true
  if( keepValue ) {
    prog->addCode( VBC1_PUSH_1, binary->mMark );
    codePrintEoln( QString("VBC1_PUSH_1") );
    }
  //Выходная метка
  binary->mExitLabel = prog->codeCount();
  codePrintEoln( QString("LAB%1:").arg(binary->mExitLabel) );
  }







void SrVpuCompiler::gvvLogOr(SrProgramm *prog, SrValueLogOr *binary, bool keepValue, bool address)
  {
  Q_ASSERT( binary != nullptr );
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
      prog->addCodeParam24( VBC4_TRUE_JUMP_KEEP, binary->mExitLabel, binary->mMark );
      codePrintEoln( QString("VBC4_TRUE_JUMP_KEEP LAB%1").arg(binary->mExitLabel) );
      }
    else {
      prog->addCodeParam24( VBC4_TRUE_JUMP, binary->mExitLabel, binary->mMark );
      codePrintEoln( QString("VBC4_TRUE_JUMP LAB%1").arg(binary->mExitLabel) );
      }
    }
  //Если прошли все, то true
  if( keepValue ) {
    prog->addCode( VBC1_PUSH_0, binary->mMark );
    codePrintEoln( QString("VBC1_PUSH_0") );
    }
  //Выходная метка
  binary->mExitLabel = prog->codeCount();
  codePrintEoln( QString("LAB%1:").arg(binary->mExitLabel) );
  }







void SrVpuCompiler::gvvComma(SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address)
  {
  Q_ASSERT( binary != nullptr );

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
  Q_ASSERT( condition != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), condition->mMark );
    return;
    }

  Q_ASSERT( condition->mCondition != nullptr && condition->mTrue != nullptr && condition->mFalse != nullptr );

  if( condition->mConst ) {
    if( keepValue ) {
      gPushConst( prog, condition->mConstInt, condition->mMark );
      codePrintEoln( QString() );
      }
    }
  else {
    if( !condition->mCondition->isConst() ) {
      //Формировать условие
      gValue( prog, condition->mCondition, true, false );

      //Переход на ложное условие
      prog->addCodeParam24( VBC4_FALSE_JUMP, condition->mFalseAddress, condition->mMark );
      codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1").arg(condition->mFalseAddress) );
      }

    if( !condition->mCondition->isConst() || condition->mCondition->toConstInt() ) {
      //Ветка true
      gValue( prog, condition->mTrue, keepValue, false );

      if( condition->mTrue->mConst && condition->mCondition->isConst() ) {
        condition->mConst = true;
        condition->mConstInt = condition->mTrue->mConstInt;
        }

      if( !condition->mCondition->isConst() ) {
        prog->addCodeParam24( VBC4_JUMP, condition->mExitAddress, condition->mMark );
        codePrintEoln( QString("VBC4_JUMP LAB%1").arg(condition->mExitAddress) );
        }
      }

    condition->mFalseAddress = prog->codeCount();
    codePrintEoln( QString("LAB%1:").arg(condition->mFalseAddress) );

    if( !condition->mCondition->isConst() || !condition->mCondition->toConstInt() ) {
      gValue( prog, condition->mFalse, keepValue, false );

      if( condition->mFalse->mConst && condition->mCondition->isConst() ) {
        condition->mConst = true;
        condition->mConstInt = condition->mFalse->mConstInt;
        }

      condition->mExitAddress = prog->codeCount();
      codePrintEoln( QString("LAB%1:").arg(condition->mExitAddress) );
      }

    //Проверим типы
    if( !condition->mCondition->isConst() && condition->mTrue->getType() != condition->mFalse->getType() )
      errorInLine( QObject::tr("Error. Types of condition must be same"), condition->mMark );

    }

  }







void SrVpuCompiler::gvvCall(SrProgramm *prog, SrValueCall *call, bool keepValue, bool address)
  {
  Q_ASSERT( call != nullptr );

  if( address ) {
    errorInLine( QObject::tr("Error. Need LValue"), call->mMark );
    return;
    }

  Q_ASSERT( call->mFunction != nullptr );

  //Добыть адрес вызова
  gValue( prog, call->mFunction, true, false );

  //Тип вызываемой функции
  SrFunctionType *funType = call->mFunction->getType()->toFunction();

  //Проверим, функцию ли мы вызываем
  if( funType == nullptr )
    errorInLine( QObject::tr("Error. Call of nonfunction"), call->mMark );
  else {
    int resultCount = 0;
    //Если функция возвращает значение, то оставим под него место
    if( funType->mResult->isInt() || funType->mResult->isPointer() ) {
      //Возвращает целое или указатель или объект
      resultCount = 1;
      prog->addCode( VBC1_ALLOC_RESULT, call->mMark );
      codePrintEoln( QString("VBC1_ALLOC_RESULT") );
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

    for( int i = 0; i < call->mParamCount; i++ ) {
      //Подготовить очередной параметр
      gValue( prog, call->mParam[i], true, false );
      //Проверим соответствие параметров
      if( !funType->mParamList.at(i).mType->isMatchParam(call->mParam[i]->getType(), call->mParam[i]->mConst && call->mParam[i]->mConstInt == 0 ) )
        errorInLine( QObject::tr("Error. Type mismatch in parametr %1").arg(funType->mParamList.at(i).mName), call->mMark );
      }

    //Теперь вызов
    switch( call->mParamCount ) {
      case 0 :
        prog->addCode( VBC1_CALL0, call->mMark );
        codePrintEoln( QString("VBC1_CALL0") );
        break;
      case 1 :
        prog->addCode( VBC1_CALL1, call->mMark );
        codePrintEoln( QString("VBC1_CALL1") );
        break;
      case 2 :
        prog->addCode( VBC1_CALL2, call->mMark );
        codePrintEoln( QString("VBC1_CALL2") );
        break;
      case 3 :
        prog->addCode( VBC1_CALL3, call->mMark );
        codePrintEoln( QString("VBC1_CALL3") );
        break;
      case 4 :
        prog->addCode( VBC1_CALL4, call->mMark );
        codePrintEoln( QString("VBC1_CALL4") );
        break;
      case 5 :
        prog->addCode( VBC1_CALL5, call->mMark );
        codePrintEoln( QString("VBC1_CALL5") );
        break;
      case 6 :
        prog->addCode( VBC1_CALL6, call->mMark );
        codePrintEoln( QString("VBC1_CALL6") );
        break;
      case 7 :
        prog->addCode( VBC1_CALL7, call->mMark );
        codePrintEoln( QString("VBC1_CALL7") );
        break;
      default :
        prog->addCodeParam8( VBC2_CALL, call->mParamCount, call->mMark );
        codePrintEoln( QString("VBC2_CALL [%1]").arg(call->mParamCount) );
      }

    //Теперь убираем все параметры
    if( keepValue ) {
      //Параметры и адрес вызова
      gStack( prog, call->mParamCount + 1, call->mMark );
      codePrintEoln( QString("//remove call address and params") );
      }
    else {
      //Параметры, адрес вызова и результат, если есть
      gStack( prog, call->mParamCount + 1 + resultCount, call->mMark );
      codePrintEoln( QString("//remove result, call address and params") );
      }
    }
  }







