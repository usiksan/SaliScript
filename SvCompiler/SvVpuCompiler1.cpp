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
#include "SvCompiler/SvVpuCompiler.h"
#include "SvVMachine/SvVmByteCode.h"
#include <QFileInfo>
#include <QDebug>

using namespace SvCompiler6;

SvVpuCompiler::SvVpuCompiler()
  {

  }

SvVpuCompiler::~SvVpuCompiler()
  {

  }

SvProgrammPtr SvVpuCompiler::make(const QString prjPath, const QString &mainScript)
  {
  //Инициализировать компилятор
  Reset();

  //Подготовить результирующую программу
  SvProgrammPtr prog( new SvProgramm() );
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

  //Пройти по глобальным переменным
  for( SvVariable *var : mVarGlobal.mList )
    //Среди переменных выискиваем константные блоки
    if( var->mType == mTypeCBlock ) {
      //Изменить адрес блока на величину строк в таблице
      //так как блоки следуют сразу после строк
      // -1 так как мы ведем нумерацию блоков с 1
      var->mAddress += mStringTable.count() - 1;
      }

  //Генерация
  //1-й проход - вычисление констант
  mList = nullptr;
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
  codePrintEoln( QString("//Init table") );
  prog->setInitTable();
  for( SvVariable *var : mVarGlobal.mList )
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
  //Зафиксировать размер программы без учета таблиц
  prog->setVpuProgSize();

  //Экспорт символов
  codePrintEoln( QString("//Variables") );
  for( SvVariable *var : mVarGlobal.mList ) {
    if( var->mSort == tsrGlobal ) {
      //Добавить символ
      addVariable( prog.data(), QString(), var, 0 );
      }
    }

  //Обозначить точку входа
  SvFunction *mainFun = mFunGlobal.getFunction( QString("main") );
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
  prog->setVersion( macroExpansion( QString("SV_PROG_VERSION"), QString("0") ).toInt() );
  prog->setVariant( macroExpansion( QString("SV_PROG_PLC_VARIANT"), QString("0") ).toInt() );
  prog->setSignature( macroExpansion( QString("SV_SIGNATURE"), QString("saliLab rc++")) );
  prog->setStackSize0( macroExpansion( QString("SV_MAIN_STACK_SIZE"), QString("300")).toInt() );
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




SvProgrammPtr SvVpuCompiler::build(const QString prjPath, const QString &mainScript)
  {
  SvVpuCompiler compiler;
  return compiler.make( prjPath, mainScript );
  }



void SvVpuCompiler::addVariable(SvProgramm *prog, const QString prefix, SvVariable *var, int startAddress )
  {
  //Добавить саму переменную
  startAddress += var->mAddress;
  prog->addSymbol( prefix + var->mName, startAddress );
  codePrintEoln( prefix + var->mName + QString(" : ") + QString::number(startAddress) );

  if( var->mType->isStruct() ) {
    //Для переменных структур развернуть и добавить все члены
    SvStruct *str = var->mType->toStruct();
    if( str != nullptr ) {
      QString subPrefix = prefix + var->mName + QString(".");
      for( SvVariable *sub : str->mMemberList.mList )
        addVariable( prog, subPrefix, sub, startAddress );
      }
    }

  }





void SvVpuCompiler::pass(SvProgramm *prog)
  {
  prog->clear();


  //Проходим все функции и генерируем
  for( SvFunction *fun : mFunGlobal.mList ) {
    codePrintEoln( QString(";%1 %2").arg(fun->mName).arg(fun->mType->mSignature) );
    //Текущее значение счетчика равно адресу начала
    fun->mAddress = prog->codeCount();
    //Сгенерировать выделение локальных переменных
    if( fun->mLocalAmount ) {
      gStack( prog, -fun->mLocalAmount, fun->mMarkDefine );
      codePrintEoln( QString("//local vars %1").arg(fun->mLocalAmount) );
      }

    //Генерировать все операторы
    gOperatorBlock( prog, fun->mBody );
    }
  }




void SvVpuCompiler::gOperator(SvProgramm *prog, SvOperator *op)
  {
  codePrintEoln( QString(";") + op->mRemark );
  switch( op->statement() ) {
    case tstReturn :
      gOperatorReturn( prog, dynamic_cast<SvOperatorReturn*>(op) );
      break;
    case tstBlock :
      gOperatorBlock( prog, dynamic_cast<SvOperatorBlock*>(op) );
      break;
    case tstExpression :
      gOperatorExpression( prog, dynamic_cast<SvOperatorExpression*>(op) );
      break;
    case tstIf :
      gOperatorIf( prog, dynamic_cast<SvOperatorIf*>(op) );
      break;
    case tstBreak :
      gOperatorBreak( prog, dynamic_cast<SvOperatorBreak*>(op) );
      break;
    case tstContinue :
      gOperatorContinue( prog, dynamic_cast<SvOperatorContinue*>(op) );
      break;
    case tstWhile :
      gOperatorWhile( prog, dynamic_cast<SvOperatorWhile*>(op) );
      break;
    case tstDo :
      gOperatorDoWhile( prog, dynamic_cast<SvOperatorDoWhile*>(op) );
      break;
    case tstFor :
      gOperatorFor( prog, dynamic_cast<SvOperatorFor*>(op) );
      break;
    default :
      Error( QObject::tr("Undefined operator") );
    }

  }


void SvVpuCompiler::gOperatorReturn(SvProgramm *prog, SvOperatorReturn *svReturn)
  {
  if( svReturn->mResult ) {
    //Вычислить результат и сохранить в возврате
    gValue( prog, svReturn->mResult, true, false );

    prog->addCodeParam8( VBC2_POP_RESULT, svReturn->mFunction->mParamSize, svReturn->mMark );
    codePrintEoln( QString("VBC2_POP_RESULT %1").arg(svReturn->mFunction->mParamSize) );
    }

  //Выполнить возврат из функции
  prog->addCode( VBC1_RETURN, svReturn->mMark );
  codePrintEoln( QString("VBC1_RETURN") );
  }





void SvVpuCompiler::gValueCatchFun(SvProgramm *prog, SvValueCatchFun *svCatch)
  {
  //Вычислить значение маски
  gValue( prog, svCatch->mCatchMask, !svCatch->mCatchMask->mConst, false );
  if( svCatch->mCatchMask->mConst ) {
    //Маска - константное значение
    prog->addCodeParam32( VBC5_CATCH, svCatch->mCatchMask->mConstInt, svCatch->mMark );
    codePrintEoln( QString("VBC5_CATCH %1").arg(svCatch->mCatchMask->mConstInt,0,16) );
    }
  else {
    //Маска - значение в стеке
    prog->addCode( VBC1_CATCH, svCatch->mMark );
    codePrintEoln( QString("VBC1_CATCH") );
    }
  }





void SvVpuCompiler::gValueThrowFun(SvProgramm *prog, SvValueThrowFun *svThrow)
  {
  //Вычислить значение маски
  gValue( prog, svThrow->mThrowCode, !svThrow->mThrowCode->mConst, false );
  if( svThrow->mThrowCode->mConst ) {
    //Маска - константное значение
    prog->addCodeParam32( VBC5_THROW, svThrow->mThrowCode->mConstInt, svThrow->mMark );
    codePrintEoln( QString("VBC5_THROW %1").arg(svThrow->mThrowCode->mConstInt,0,16) );
    }
  else {
    //Маска - значение в стеке
    prog->addCode( VBC1_THROW, svThrow->mMark );
    codePrintEoln( QString("VBC1_THROW") );
    }
  }





void SvVpuCompiler::gOperatorBlock(SvProgramm *prog, SvOperatorBlock *svBlock)
  {
  if( svBlock == nullptr ) return;

  //Генерировать все операторы блока
  for( SvOperator *op : svBlock->mList )
    gOperator( prog, op );
  }





void SvVpuCompiler::gOperatorExpression(SvProgramm *prog, SvOperatorExpression *svExpression )
  {
  //Генерировать вычисление выражения
  gValue( prog, svExpression->mExpression, false, false );
  }





void SvVpuCompiler::gOperatorIf(SvProgramm *prog, SvOperatorIf *svIf)
  {
  if( !svIf->mCondition || !svIf->mCondition->isConst() ) {
    //Формировать условие
    gValue( prog, svIf->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svIf->mFalseAddress, svIf->mMark );
    codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1 //if to false").arg(svIf->mFalseAddress) );
    }

  if( !svIf->mCondition || !svIf->mCondition->isConst() || svIf->mCondition->toCondition() ) {
    //Ветка true
    gOperator( prog, svIf->mTrue );

    if( svIf->mFalse ) {
      prog->addCodeParam24( VBC4_JUMP, svIf->mExitAddress, svIf->mMark );
      codePrintEoln( QString("VBC4_JUMP LAB%1 //if to exit").arg(svIf->mExitAddress) );
      }
    }

  svIf->mFalseAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //if else").arg(svIf->mFalseAddress) );

  if( svIf->mFalse ) {
    gOperator( prog, svIf->mFalse );

    svIf->mExitAddress = prog->codeCount();
    codePrintEoln( QString("LAB%1: //if exit").arg(svIf->mExitAddress) );
    }
  }




void SvVpuCompiler::gOperatorBreak(SvProgramm *prog, SvOperatorBreak *svBreak)
  {
  prog->addCodeParam24( VBC4_JUMP, svBreak->mLoop->mExitAddress, svBreak->mMark );
  codePrintEoln( QString("VBC4_JUMP LAB%1 //to loop exit").arg(svBreak->mLoop->mExitAddress) );
  }




void SvVpuCompiler::gOperatorContinue(SvProgramm *prog, SvOperatorContinue *svContinue)
  {
  prog->addCodeParam24( VBC4_JUMP, svContinue->mLoop->mConditionAddress, svContinue->mMark );
  codePrintEoln( QString("VBC4_JUMP LAB%1 //to loop condition").arg(svContinue->mLoop->mConditionAddress) );
  }





void SvVpuCompiler::gOperatorWhile(SvProgramm *prog, SvOperatorWhile *svWhile)
  {
  //Начало цикла - проверка условия
  svWhile->mConditionAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //while condition").arg(svWhile->mConditionAddress) );

  if( !svWhile->mCondition || !svWhile->mCondition->isConst() ) {
    //Вычислить условие
    gValue( prog, svWhile->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svWhile->mExitAddress, svWhile->mMark );
    codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1 //while to exit").arg(svWhile->mExitAddress) );

    }
  //Условие константное, либо зацикливание, либо вообще ничего делать не нужно

  if( !svWhile->mCondition || !svWhile->mCondition->isConst() || svWhile->mCondition->toCondition() ) {
    //Тело
    gOperator( prog, svWhile->mBody );

    //зацикливание
    prog->addCodeParam24( VBC4_JUMP, svWhile->mConditionAddress, svWhile->mMark );
    codePrintEoln( QString("VBC4_JUMP LAB%1 //while forever loop").arg(svWhile->mConditionAddress) );
    }

  svWhile->mExitAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //while exit").arg(svWhile->mExitAddress) );
  }






void SvVpuCompiler::gOperatorDoWhile(SvProgramm *prog, SvOperatorDoWhile *svDoWhile)
  {
  int start = prog->codeCount();
  codePrintEoln( QString("LAB%1: //do while body").arg(start) );

  //Тело
  gOperator( prog, svDoWhile->mBody );

  //Проверка условия
  svDoWhile->mConditionAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //do while condition").arg(svDoWhile->mConditionAddress) );

  if( svDoWhile->mCondition && svDoWhile->mCondition->isConst() ) {
    //Постоянное условие, вычислять не нужно
    //Либо выход, либо зацикливание
    if( !svDoWhile->mCondition->toCondition() ) {
      //Безусловное зацикливание
      prog->addCodeParam24( VBC4_JUMP, start, svDoWhile->mMark );
      codePrintEoln( QString("VBC4_JUMP LAB%1 //forever loop").arg(start) );
      }
    }
  else {
    gValue( prog, svDoWhile->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, start, svDoWhile->mMark );
    codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1 //do while to body").arg(start) );
    }

  svDoWhile->mExitAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //do while exit").arg(svDoWhile->mExitAddress) );
  }






void SvVpuCompiler::gOperatorFor(SvProgramm *prog, SvOperatorFor *svFor)
  {

  //Инициализация
  gValue( prog, svFor->mInit, false, false );

  int start = prog->codeCount();
  codePrintEoln( QString("LAB%1: //for condition").arg(start) );

  if( !svFor->mCondition || !svFor->mCondition->isConst() ) {
    //Вычислить условие
    gValue( prog, svFor->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VBC4_FALSE_JUMP, svFor->mExitAddress, svFor->mMark );
    codePrintEoln( QString("VBC4_FALSE_JUMP LAB%1 //to for exit").arg(svFor->mExitAddress) );
    }

  //Тело цикла не требуется, когда константное условие ложно
  if( !svFor->mCondition || !svFor->mCondition->isConst() || svFor->mCondition->toCondition() ) {
    //Требуется тело цикла

    //Тело
    gOperator( prog, svFor->mBody );

    //Инкремент
    svFor->mConditionAddress = prog->codeCount();
    codePrintEoln( QString("LAB%1: //for step").arg(svFor->mConditionAddress) );

    gValue( prog, svFor->mAction, false, false );

    //Переход на сравнение
    prog->addCodeParam24( VBC4_JUMP, start, svFor->mMark );
    codePrintEoln( QString("VBC4_JUMP LAB%1 //to condition").arg(start) );
    }

  svFor->mExitAddress = prog->codeCount();
  codePrintEoln( QString("LAB%1: //for exit").arg(svFor->mExitAddress) );
  }















void SvVpuCompiler::gPushConst(SvProgramm *prog, int val, const SvMark &mark )
  {
  switch( val ) {
    case 0 :
      prog->addCode( VBC1_PUSH_0, mark );
      codePrint( QString("VBC1_PUSH_0 ") );
      break;
    case 1 :
      prog->addCode( VBC1_PUSH_1, mark );
      codePrint( QString("VBC1_PUSH_1 ") );
      break;
    case 2 :
      prog->addCode( VBC1_PUSH_2, mark );
      codePrint( QString("VBC1_PUSH_2 ") );
      break;
    default :
      if( val >= -128 && val <= 127 ) {
        prog->addCodeParam8( VBC2_PUSH_CONST, val, mark );
        codePrint( QString("VBC2_PUSH_CONST %1 ").arg(val) );
        }
      else if( val >= -32768 && val <= 32767 ) {
        prog->addCodeParam16( VBC3_PUSH_CONST, val, mark );
        codePrint( QString("VBC3_PUSH_CONST %1 ").arg(val) );
        }
      else if( val >= -8388608 && val <= 8388607 ) {
        prog->addCodeParam24( VBC4_PUSH_CONST, val, mark );
        codePrint( QString("VBC4_PUSH_CONST %1 ").arg(val) );
        }
      else {
        prog->addCodeParam32( VBC5_PUSH_CONST, val, mark );
        codePrint( QString("VBC5_PUSH_CONST %1 ").arg(val) );
        }
    }
  }







void SvVpuCompiler::gStack(SvProgramm *prog, int offset, const SvMark &mark)
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






void SvVpuCompiler::gLoad(SvProgramm *prog, SvValue *val)
  {
  prog->addCode( VBC1_LOAD, val->mMark );
  codePrint( QString("VBC1_LOAD") );
  }




//! Если задан файл листинга, то выводит в него заданную строку
void SvVpuCompiler::codePrint(const QString str)
  {
  if( mList )
    (*mList) << str;
  }




//! Если задан файл листинга, то выводит в него строку и символ новой строки
void SvVpuCompiler::codePrintEoln(const QString str)
  {
  if( mList )
    (*mList) << str << '\n';
  }











