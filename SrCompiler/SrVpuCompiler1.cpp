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
#include <QFileInfo>
#include <QDebug>

using namespace SrCompiler6;

SrVpuCompiler::SrVpuCompiler()
  {

  }

SrVpuCompiler::~SrVpuCompiler()
  {

  }

SrProgrammPtr SrVpuCompiler::make(const QString prjPath, const QString &mainScript)
  {
  //Инициализировать компилятор
  Reset();

  //Подготовить результирующую программу
  SrProgrammPtr prog( new SrProgramm() );
  if( !QFile::exists(prjPath + mainScript) ) {
    Error( QObject::tr("Can't open main script file ").append(mainScript) );
    //Скопировать перечень ошибок
    prog->mErrors = mErrorList;
    return prog;
    }
  //Некоторые начальные установки
  mProjectPath = prjPath;

  //Компиляция
  Compile( mainScript );

  //Вывести листинг в файл
  QFileInfo listingFileInfo( mainScript );
  QFile listingFile( prjPath + listingFileInfo.completeBaseName() + ".lst" );
  if( listingFile.open(QIODevice::WriteOnly) ) {
    //Листинг идет
    listingFile.write( Listing().toUtf8() );
    listingFile.close();
    }

  if( !mErrorList.isEmpty() ) {
    //Скопировать перечень ошибок
    prog->mErrors = mErrorList;
    return prog;
    }

  //Генерация
  //1-й проход - вычисление констант
  mList = 0;
  pass( prog.data() );
  if( !mErrorList.isEmpty() ) {
    //После 1-го прохода возникли ошибки, составить новый листинг
    if( listingFile.open(QIODevice::WriteOnly) ) {
      //Листинг идет
      listingFile.write( Listing().toUtf8() );
      listingFile.close();
      }
    //Скопировать перечень ошибок
    prog->mErrors = mErrorList;
    return prog;
    }

  //2-й проход - формирование адресов
  pass( prog.data() );
  if( !mErrorList.isEmpty() ) {
    //После 2-го прохода возникли ошибки, составить новый листинг
    if( listingFile.open(QIODevice::WriteOnly) ) {
      //Листинг идет
      listingFile.write( Listing().toUtf8() );
      listingFile.close();
      }
    //Скопировать перечень ошибок
    prog->mErrors = mErrorList;
    return prog;
    }

  //3-й проход - окончательная генерация
  QFile listFile( prjPath + listingFileInfo.completeBaseName() + ".list" );
  listFile.open(QIODevice::WriteOnly);
  QTextStream lst( &listFile );
  mList = &lst;

  pass( prog.data() );



  //Для всех статических объектов сформировать конструирование
  codePrint( QString("Init table") );
  prog->setInitTable();
  for( SrVariable *var : mVarGlobal.mList )
    if( var->mInit ) {
      //Обеспечить константное выражение
      gValue( prog.data(), var->mInit, false, false );
      if( var->mInit->isConst() ) {
        prog->add24( var->mAddress, 0, 0 );
        prog->add32( var->mInit->mConstInt, 0, 0 );
        codePrint( QString("%1[%2]= %3").arg(var->mName).arg(var->mAddress).arg(var->mInit->mConstInt) );
        }
      else errorInLine( QObject::tr("Error. Must be constant expression"), var->mInit->mMark );
      }

  //Закончить таблицу
  prog->add24( 0, 0, 0 );

  //Экспорт символов
  for( SrVariable *var : mVarGlobal.mList ) {
    if( var->mSort == tsrGlobal ) {
      //Добавить символ
      addVariable( prog.data(), QString(), var, 0 );
      }
    }

  //Обозначить точку входа
  SrFunction *mainFun = mFunGlobal.getFunction( QString("main") );
  if( mainFun ) {
    prog->setEntry( mainFun->mAddress );
    }
  else {
    Error( QObject::tr("main function not defined") );
    }

  // [TODO] Разместить таблицу строк внутри программы
  prog->mStrings = mStringTable;
  prog->mErrors  = mErrorList;


  //Записать необходимую версию контроллера
  prog->setVersion( macroExpansion( QString("PROG_VERSION"), QString("0") ).toInt() );
  prog->setVariant( macroExpansion( QString("PROG_PLC_VARIANT"), QString("0") ).toInt() );
  prog->setSignature( macroExpansion( QString("SV_SIGNATURE"), QString("saliLab rc++")) );
  prog->setStackSize0( macroExpansion( QString("MAIN_STACK_SIZE"), QString("300")).toInt() );
  prog->setProgSize();
  prog->setGlobalCount( mGlobalAddress );

  listFile.close();

  //Сохраним бинарный файл
  QFile binFile( prjPath + listingFileInfo.completeBaseName() + ".vpu" );
  if( binFile.open(QIODevice::WriteOnly) ) {
    binFile.write( prog->toArray() );
    binFile.close();
    }

  return prog;
  }



void SrVpuCompiler::addVariable(SrProgramm *prog, const QString prefix, SrVariable *var, int startAddress )
  {
  //Добавить саму переменную
  startAddress += var->mAddress;
  prog->addSymbol( prefix + var->mName, startAddress );
  codePrint( prefix + var->mName + QString(" : ") + QString::number(startAddress) );

  if( var->mType->isStruct() ) {
    //Для переменных структур развернуть и добавить все члены
    SrStruct *str = var->mType->toStruct();
    if( str != nullptr ) {
      QString subPrefix = prefix + var->mName + QString(".");
      for( SrVariable *sub : str->mMemberList.mList )
        addVariable( prog, subPrefix, sub, startAddress );
      }
    }

  }





void SrVpuCompiler::pass(SrProgramm *prog)
  {
  prog->clear();


  //Проходим все функции и генерируем
  for( SrFunction *fun : mFunGlobal.mList ) {
    codePrint( QString(";%1 %2").arg(fun->mName).arg(fun->mType->mSignature) );
    //Текущее значение счетчика равно адресу начала
    fun->mAddress = prog->codeCount();
    //Сгенерировать выделение локальных переменных
    gStack( prog, -fun->mLocalAmount, fun->mMarkDefine );

    //Генерировать все операторы
    gOperatorBlock( prog, fun->mBody );
    }
  }




void SrVpuCompiler::gOperator(SrProgramm *prog, SrOperator *op)
  {
  codePrint( QString(";") + op->mRemark );
  switch( op->statement() ) {
    case tstReturn :
      gOperatorReturn( prog, dynamic_cast<SrOperatorReturn*>(op) );
      break;
    case tstBlock :
      gOperatorBlock( prog, dynamic_cast<SrOperatorBlock*>(op) );
      break;
    case tstExpression :
      gOperatorExpression( prog, dynamic_cast<SrOperatorExpression*>(op) );
      break;
    case tstIf :
      gOperatorIf( prog, dynamic_cast<SrOperatorIf*>(op) );
      break;
    case tstBreak :
      gOperatorBreak( prog, dynamic_cast<SrOperatorBreak*>(op) );
      break;
    case tstContinue :
      gOperatorContinue( prog, dynamic_cast<SrOperatorContinue*>(op) );
      break;
    case tstWhile :
      gOperatorWhile( prog, dynamic_cast<SrOperatorWhile*>(op) );
      break;
    case tstDo :
      gOperatorDoWhile( prog, dynamic_cast<SrOperatorDoWhile*>(op) );
      break;
    case tstFor :
      gOperatorFor( prog, dynamic_cast<SrOperatorFor*>(op) );
      break;
    default :
      Error( QObject::tr("Undefined operator") );
    }

  }


void SrVpuCompiler::gOperatorReturn(SrProgramm *prog, SrOperatorReturn *svReturn)
  {
  if( svReturn->mResult ) {
    //Вычислить результат и сохранить в возврате
    gValue( prog, svReturn->mResult, true, false );

    prog->addCodeParam8( VBC2_POP_RESULT, svReturn->mFunction->mParamSize, svReturn->mMark );
    codePrint( QString("VBC2_POP_RESULT %1").arg(svReturn->mFunction->mParamSize) );
    }

  //Выполнить возврат из функции
  prog->addCode( VBC1_RETURN, svReturn->mMark );
  codePrint( QString("VBC1_RETURN") );
  }





void SrVpuCompiler::gValueCatchFun(SrProgramm *prog, SrValueCatchFun *svCatch)
  {
  //Вычислить значение маски
  gValue( prog, svCatch->mCatchMask, !svCatch->mCatchMask->mConst, false );
  if( svCatch->mCatchMask->mConst ) {
    //Маска - константное значение
    prog->addCodeParam32( VBC5_CATCH, svCatch->mCatchMask->mConstInt, svCatch->mMark );
    codePrint( QString("VBC5_CATCH %1").arg(svCatch->mCatchMask->mConstInt,0,16) );
    }
  else {
    //Маска - значение в стеке
    prog->addCode( VBC1_CATCH, svCatch->mMark );
    codePrint( QString("VBC1_CATCH") );
    }
  }





void SrVpuCompiler::gValueThrowFun(SrProgramm *prog, SrValueThrowFun *svThrow)
  {
  //Вычислить значение маски
  gValue( prog, svThrow->mThrowCode, !svThrow->mThrowCode->mConst, false );
  if( svThrow->mThrowCode->mConst ) {
    //Маска - константное значение
    prog->addCodeParam32( VBC5_THROW, svThrow->mThrowCode->mConstInt, svThrow->mMark );
    codePrint( QString("VBC5_THROW %1").arg(svThrow->mThrowCode->mConstInt,0,16) );
    }
  else {
    //Маска - значение в стеке
    prog->addCode( VBC1_THROW, svThrow->mMark );
    codePrint( QString("VBC1_THROW") );
    }
  }





void SrVpuCompiler::gOperatorBlock(SrProgramm *prog, SrOperatorBlock *svBlock)
  {
  if( svBlock == nullptr ) return;

  //Пролог блока
  gOperatorContextEnter( prog, svBlock );

  //Генерировать все операторы блока
  for( SrOperator *op : svBlock->mList )
    gOperator( prog, op );
  }





void SrVpuCompiler::gOperatorExpression(SrProgramm *prog, SrOperatorExpression *svExpression )
  {
  //Генерировать вычисление выражения
  gValue( prog, svExpression->mExpression, false, false );
  }





void SrVpuCompiler::gOperatorIf(SrProgramm *prog, SrOperatorIf *svIf)
  {
  if( !svIf->mCondition || !svIf->mCondition->isConst() ) {
    //Формировать условие
    gValue( prog, svIf->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svIf->mFalseAddress, svIf->mMark );
    codePrint( QString("VBC4_FALSE_JUMP LAB%1").arg(svIf->mFalseAddress) );
    }

  if( !svIf->mCondition || !svIf->mCondition->isConst() || svIf->mCondition->toCondition() ) {
    //Ветка true
    gOperator( prog, svIf->mTrue );

    if( svIf->mFalse ) {
      prog->addCodeParam24( VBC4_JUMP, svIf->mExitAddress, svIf->mMark );
      codePrint( QString("VBC4_JUMP LAB%1").arg(svIf->mExitAddress) );
      }
    }

  svIf->mFalseAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svIf->mFalseAddress) );

  if( svIf->mFalse ) {
    gOperator( prog, svIf->mFalse );

    svIf->mExitAddress = prog->codeCount();
    codePrint( QString("LAB%1:").arg(svIf->mExitAddress) );
    }
  }




void SrVpuCompiler::gOperatorBreak(SrProgramm *prog, SrOperatorBreak *svBreak)
  {
  prog->addCodeParam24( VBC4_JUMP, svBreak->mLoop->mExitAddress, svBreak->mMark );
  codePrint( QString("VBC4_JUMP LAB%1").arg(svBreak->mLoop->mConditionAddress) );
  }




void SrVpuCompiler::gOperatorContinue(SrProgramm *prog, SrOperatorContinue *svContinue)
  {
  prog->addCodeParam24( VBC4_JUMP, svContinue->mLoop->mConditionAddress, svContinue->mMark );
  codePrint( QString("VBC4_JUMP LAB%1").arg(svContinue->mLoop->mConditionAddress) );
  }





void SrVpuCompiler::gOperatorWhile(SrProgramm *prog, SrOperatorWhile *svWhile)
  {
  //Начало цикла - проверка условия
  svWhile->mConditionAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svWhile->mConditionAddress) );

  if( !svWhile->mCondition || !svWhile->mCondition->isConst() ) {
    //Вычислить условие
    gValue( prog, svWhile->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svWhile->mExitAddress, svWhile->mMark );
    codePrint( QString("VBC4_FALSE_JUMP LAB%1").arg(svWhile->mExitAddress) );

    }
  //Условие константное, либо зацикливание, либо вообще ничего делать не нужно

  if( !svWhile->mCondition || !svWhile->mCondition->isConst() || svWhile->mCondition->toCondition() ) {
    //Тело
    gOperator( prog, svWhile->mBody );

    //зацикливание
    prog->addCodeParam24( VBC4_JUMP, svWhile->mConditionAddress, svWhile->mMark );
    codePrint( QString("VBC4_JUMP LAB%1").arg(svWhile->mConditionAddress) );
    }

  svWhile->mExitAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svWhile->mExitAddress) );
  }






void SrVpuCompiler::gOperatorDoWhile(SrProgramm *prog, SrOperatorDoWhile *svDoWhile)
  {
  int start = prog->codeCount();
  codePrint( QString("LAB%1:").arg(start) );

  //Тело
  gOperator( prog, svDoWhile->mBody );

  //Проверка условия
  svDoWhile->mConditionAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svDoWhile->mConditionAddress) );

  if( svDoWhile->mCondition && svDoWhile->mCondition->isConst() ) {
    //Постоянное условие, вычислять не нужно
    //Либо выход, либо зацикливание
    if( !svDoWhile->mCondition->toCondition() )
      //Безусловное зацикливание
      prog->addCodeParam24( VBC4_JUMP, start, svDoWhile->mMark );
      codePrint( QString("VBC4_JUMP LAB%1").arg(start) );
    }
  else {
    gValue( prog, svDoWhile->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, start, svDoWhile->mMark );
    codePrint( QString("VBC4_FALSE_JUMP LAB%1").arg(start) );
    }

  svDoWhile->mExitAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svDoWhile->mExitAddress) );
  }






void SrVpuCompiler::gOperatorFor(SrProgramm *prog, SrOperatorFor *svFor)
  {
  //Пролог
  gOperatorContextEnter( prog, svFor );

  //Инициализация
  gValue( prog, svFor->mInit, false, false );

  int start = prog->codeCount();
  codePrint( QString("LAB%1:").arg(start) );

  if( !svFor->mCondition || !svFor->mCondition->isConst() ) {
    //Вычислить условие
    gValue( prog, svFor->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svFor->mExitAddress, svFor->mMark );
    codePrint( QString("VBC4_FALSE_JUMP LAB%1").arg(svFor->mExitAddress) );
    }

  //Тело цикла не требуется, когда константное условие ложно
  if( !svFor->mCondition || !svFor->mCondition->isConst() || svFor->mCondition->toCondition() ) {
    //Требуется тело цикла

    //Тело
    gOperator( prog, svFor->mBody );

    //Инкремент
    svFor->mConditionAddress = prog->codeCount();
    codePrint( QString("LAB%1:").arg(svFor->mConditionAddress) );

    gValue( prog, svFor->mAction, false, false );

    //Переход на сравнение
    prog->addCodeParam24( VBC4_JUMP, start, svFor->mMark );
    codePrint( QString("VBC4_JUMP LAB%1").arg(start) );
    }

  svFor->mExitAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svFor->mExitAddress) );
  }







//Создать локальные переменные при входе в блок
void SrVpuCompiler::gOperatorContextEnter(SrProgramm *prog, SrOperatorContext *context)
  {
  //Последовательно для всех переменных выполняем создание
  for( SrVariable *var : context->mVarLocal.mList ) {

    //Проверить наличие инициализационного выражения
    if( var->mInit ) {
      //Есть инициализационное выражение
      prog->addCodeParam8( VBC2_PUSH_B_OFFSET, var->mAddress, var->mMarkDefine );
      codePrint( QString("VBC2_PUSH_B_OFFSET ") + var->mName + QString("<%1>").arg( var->mAddress ) );
      //Теперь значение для сохранения
      gValue( prog, var->mInit, true, false );
      //Теперь сохранение
      if( var->mType->canAssign(var->mInit->mType) ) {
        prog->addCode( VBC1_POP, var->mMarkDefine );
        codePrint( QString("VBC1_POP") );
        }
      else
        errorInLine( QObject::tr("Error. Can't assign %1 to %2").arg(var->mInit->mType->mName).arg(var->mType->mName), var->mMarkDefine );
      }

    }

  }









void SrVpuCompiler::gPushConst(SrProgramm *prog, int val, const SrMark &mark, const QString label )
  {
  switch( val ) {
    case 0 :
      prog->addCode( VBC1_PUSH_0, mark );
      codePrint( QString("VBC1_PUSH_0 [%1]").arg(label) );
      break;
    case 1 :
      prog->addCode( VBC1_PUSH_1, mark );
      codePrint( QString("VBC1_PUSH_1 [%1]").arg(label) );
      break;
    case 2 :
      prog->addCode( VBC1_PUSH_2, mark );
      codePrint( QString("VBC1_PUSH_2 [%1]").arg(label) );
      break;
    default :
      if( val >= -128 && val <= 127 ) {
        prog->addCodeParam8( VBC2_PUSH_CONST, val, mark );
        codePrint( QString("VBC2_PUSH_CONST %1 [%2]").arg(val).arg(label) );
        }
      else if( val >= -32768 && val <= 32767 ) {
        prog->addCodeParam16( VBC3_PUSH_CONST, val, mark );
        codePrint( QString("VBC3_PUSH_CONST %1 [%2]").arg(val).arg(label) );
        }
      else if( val >= -8388608 && val <= 8388607 ) {
        prog->addCodeParam24( VBC4_PUSH_CONST, val, mark );
        codePrint( QString("VBC4_PUSH_CONST %1 [%2]").arg(val).arg(label) );
        }
      else {
        prog->addCodeParam32( VBC5_PUSH_CONST, val, mark );
        codePrint( QString("VBC5_PUSH_CONST %1 [%2]").arg(val).arg(label) );
        }
    }
  }







void SrVpuCompiler::gStack(SrProgramm *prog, int offset, const SrMark &mark)
  {
  switch( offset ) {
    case 0 : break;
    case -1 :
      prog->addCode( VBC1_STACK_DN1, mark );
      codePrint( "VBC1_STACK_DN1");
      break;
    case -2 :
      prog->addCode( VBC1_STACK_DN2, mark );
      codePrint( "VBC1_STACK_DN2");
      break;

    case 1 :
      prog->addCode( VBC1_STACK_UP1, mark );
      codePrint( "VBC1_STACK_UP1");
      break;
    case 2 :
      prog->addCode( VBC1_STACK_UP2, mark );
      codePrint( "VBC1_STACK_UP2");
      break;
    case 3 :
      prog->addCode( VBC1_STACK_UP3, mark );
      codePrint( "VBC1_STACK_UP3");
      break;
    case 4 :
      prog->addCode( VBC1_STACK_UP4, mark );
      codePrint( "VBC1_STACK_UP4");
      break;
    case 5 :
      prog->addCode( VBC1_STACK_UP5, mark );
      codePrint( "VBC1_STACK_UP5");
      break;
    case 6 :
      prog->addCode( VBC1_STACK_UP6, mark );
      codePrint( "VBC1_STACK_UP6");
      break;
    case 7 :
      prog->addCode( VBC1_STACK_UP7, mark );
      codePrint( "VBC1_STACK_UP7");
      break;
    case 8 :
      prog->addCode( VBC1_STACK_UP8, mark );
      codePrint( "VBC1_STACK_UP8");
      break;

    default :
      if( offset <= 127 && offset > -127 ) {
        prog->addCodeParam8( VBC2_STACK, offset, mark );
        codePrint( QString("VBC2_STACK ") + QString::number(offset) );
        }
      else {
        prog->addCodeParam16( VBC3_STACK, offset, mark );
        codePrint( QString("VBC3_STACK ") + QString::number(offset) );
        }
    }

  }






void SrVpuCompiler::gLoad(SrProgramm *prog, SrValue *val)
  {
  prog->addCode( VBC1_LOAD, val->mMark );
  codePrint( QString("VBC1_LOAD") );
  }




void SrVpuCompiler::codePrint(const QString code)
  {
  if( mList )
    (*mList) << code << '\n';
  }








