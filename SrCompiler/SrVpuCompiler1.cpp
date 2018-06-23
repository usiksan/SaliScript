/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvVpuCompiler - vpu back end generator
*/

#include "SrVpuCompiler.h"
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

  DoSceneParsing();

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



  //Сформировать виртуальные таблицы методов
  for( SrType *type : mTypeList.mList )
    if( type && type->mClass & TTYPE_STRUCT ) {
      SrClass *cls = type->toClass();
      //Формировать таблицу
      codePrint( cls->mName );
      cls->mVTable = prog->codeCount();
      for( SrFunction *fun : cls->mFunctionList.mList ) {
        if( fun->mAddress ) {
          prog->add24( fun->mAddress, 0, 0 );
          codePrint( QString("[%1] %2").arg(fun->mAddress).arg(fun->mName) );
          }
        else {
          Error( QObject::tr("Member %1 not defined").arg(fun->mName) );
          }
        }
      }

  //Для всех статических объектов сформировать конструирование
  codePrint( QString("Init table") );
  prog->setInitTable();
  for( SrVariable *var : mVarGlobal.mList )
    if( var->mType->mClass & TTYPE_STRUCT && var->mSort == tsrGlobal ) {
      prog->add24( var->mAddress, 0, 0 );
      prog->add32( var->mType->toClass()->mVTable, 0, 0 );
      codePrint( QString("%1-%2 %3-%4").arg(var->mAddress).arg(var->mType->toClass()->mVTable).arg(var->mName).arg(var->mType->mName) );
      }
    else if( var->mInit ) {
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
      prog->addSymbol( var->mName, var->mAddress );
      //prog->AddSymbol( var.mName, i->mAddress, i->mType->mType != TTYPE_FUNCTION && i->mAddress < mGlobalAddress, i->mType->mType == TTYPE_ARRAY ? i->mType->mNumElem : 1 );
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
  prog->setVersion( mMacroTable.value( QString("PROG_VERSION"), QString("0") ).toInt() );
  prog->setVariant( mMacroTable.value( QString("PROG_PLC_VARIANT"), QString("0") ).toInt() );
  prog->setSignature( mMacroTable.value( QString("SV_SIGNATURE"), QString("SvSmlCreator")) );
  prog->setStackSize0( mMacroTable.value( QString("MAIN_STACK_SIZE"), QString("300")).toInt() );
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





void SrVpuCompiler::pass(SrProgramm *prog)
  {
  prog->clear();


  //Проходим все функции и генерируем
  for( SrFunction *fun : mFunGlobal.mList ) {
    codePrint( QString(";%1 %2").arg(fun->mName).arg(fun->mType->mSignature) );
    //Текущее значение счетчика равно адресу начала
    fun->mAddress = prog->codeCount();
    if( fun->mStruct ) {
      //Функция принадлежит структуре, обновить адрес
      SrFunction *fmem = fun->mStruct->getFunction( fun->mName );
      if( fmem )
        fmem->mAddress = fun->mAddress;
      }
    //Сгенерировать выделение локальных переменных
    gStack( prog, -fun->mLocalAmount, fun->mMarkDefine );

    //Генерировать все операторы
    gOperatorBlock( prog, fun->mBody );
    }

  //Проверим наличие программы сцен
  if( mPropertyPtrList.count() ) {
    //Формирование таблицы свойств

    //Записать начало таблицы свойств
    prog->setPropTable();
    prog->setPropCount( mPropertyPtrList.count() );
    codePrint( QString(";Property table") );
    for( SrSmlPropertyPtr ptr : mPropertyPtrList )
      gProperty( prog, ptr );


    //Формирование таблицы сцен

    //Записать начало таблицы сцен
    prog->setSceneTable();
    prog->setSceneCount( mScenePtrList.count() );
    codePrint( QString(";Scene table") );
    for( SrSmlScene *scene : mScenePtrList )
      gScene( prog, scene );


    //Построить binding выражения
    codePrint( QString(";Binding expressions") );
    int i = 0;
    for( SrOperator *op : mBindingList ) {
      SrOperatorBinding *bind = dynamic_cast<SrOperatorBinding*>(op);
      i++;
      if( bind->mUsed ) {
        codePrint( QString(";binding %1 [%2]").arg(i).arg(prog->codeCount()) );
        //Сохранить адрес binding-выражения
        bind->mAddress = prog->codeCount();
        gValue( prog, bind->mBinding, true, false );
        //Завершить binding
        prog->addCode( VPC1_STOP_BIND, bind->mMark );
        codePrint( QString("VPC1_STOP_BIND") );
        }
      }
    }
  }




void SrVpuCompiler::gOperator(SrProgramm *prog, SrOperator *op)
  {
  codePrint( QString(";") + op->mRemark );
  switch( op->statement() ) {
    case tstReturn :
      gOperatorReturn( prog, dynamic_cast<SrOperatorReturn*>(op) );
      break;
    case tstCatch :
      gOperatorCatch( prog, dynamic_cast<SrOperatorCatch*>(op) );
      break;
    case tstThrow :
      gOperatorThrow( prog, dynamic_cast<SrOperatorThrow*>(op) );
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

    prog->addCodeParam8( VPC2_POP_RESULT, svReturn->mFunction->mParamSize, svReturn->mMark );
    codePrint( QString("VPC2_POP_RESULT %1").arg(svReturn->mFunction->mParamSize) );
    }

  //Убрать все объекты
  gOperatorTreeContextExit( prog, svReturn );
  //Выполнить возврат из функции
  prog->addCode( VPC1_RETURN, svReturn->mMark );
  codePrint( QString("VPC1_RETURN") );
  }





void SrVpuCompiler::gOperatorCatch(SrProgramm *prog, SrOperatorCatch *svCatch)
  {
  prog->addCodeParam32( VPC5_CATCH, svCatch->mMask, svCatch->mMark );
  codePrint( QString("VPC5_CATCH %1").arg(svCatch->mMask,0,16) );
  }





void SrVpuCompiler::gOperatorThrow(SrProgramm *prog, SrOperatorThrow *svThrow)
  {
  prog->addCodeParam32( VPC5_THROW, svThrow->mMask, svThrow->mMark );
  codePrint( QString("VPC5_THROW %1").arg(svThrow->mMask,0,16) );
  }





void SrVpuCompiler::gOperatorBlock(SrProgramm *prog, SrOperatorBlock *svBlock)
  {
  if( svBlock == 0 ) return;

  //Пролог блока
  gOperatorContextEnter( prog, svBlock );

  //Генерировать все операторы блока
  for( SrOperator *op : svBlock->mList )
    gOperator( prog, op );

  //Эпилог блока
  gOperatorContextExit( prog, svBlock );
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
    prog->addCodeParam24( VPC4_FALSE_JUMP, svIf->mFalseAddress, svIf->mMark );
    codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(svIf->mFalseAddress) );
    }

  if( !svIf->mCondition || !svIf->mCondition->isConst() || svIf->mCondition->toCondition() ) {
    //Ветка true
    gOperator( prog, svIf->mTrue );

    if( svIf->mFalse ) {
      prog->addCodeParam24( VPC4_JUMP, svIf->mExitAddress, svIf->mMark );
      codePrint( QString("VPC4_JUMP LAB%1").arg(svIf->mExitAddress) );
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
  prog->addCodeParam24( VPC4_JUMP, svBreak->mLoop->mExitAddress, svBreak->mMark );
  codePrint( QString("VPC4_JUMP LAB%1").arg(svBreak->mLoop->mConditionAddress) );
  }




void SrVpuCompiler::gOperatorContinue(SrProgramm *prog, SrOperatorContinue *svContinue)
  {
  prog->addCodeParam24( VPC4_JUMP, svContinue->mLoop->mConditionAddress, svContinue->mMark );
  codePrint( QString("VPC4_JUMP LAB%1").arg(svContinue->mLoop->mConditionAddress) );
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
    prog->addCodeParam24( VPC4_FALSE_JUMP, svWhile->mExitAddress, svWhile->mMark );
    codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(svWhile->mExitAddress) );

    }
  //Условие константное, либо зацикливание, либо вообще ничего делать не нужно

  if( !svWhile->mCondition || !svWhile->mCondition->isConst() || svWhile->mCondition->toCondition() ) {
    //Тело
    gOperator( prog, svWhile->mBody );

    //зацикливание
    prog->addCodeParam24( VPC4_JUMP, svWhile->mConditionAddress, svWhile->mMark );
    codePrint( QString("VPC4_JUMP LAB%1").arg(svWhile->mConditionAddress) );
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
      prog->addCodeParam24( VPC4_JUMP, start, svDoWhile->mMark );
      codePrint( QString("VPC4_JUMP LAB%1").arg(start) );
    }
  else {
    gValue( prog, svDoWhile->mCondition, true, false );

    //Переход на ложное условие
    prog->addCodeParam24( VPC4_FALSE_JUMP, start, svDoWhile->mMark );
    codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(start) );
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
    prog->addCodeParam24( VPC4_FALSE_JUMP, svFor->mExitAddress, svFor->mMark );
    codePrint( QString("VPC4_FALSE_JUMP LAB%1").arg(svFor->mExitAddress) );
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
    prog->addCodeParam24( VPC4_JUMP, start, svFor->mMark );
    codePrint( QString("VPC4_JUMP LAB%1").arg(start) );
    }

  svFor->mExitAddress = prog->codeCount();
  codePrint( QString("LAB%1:").arg(svFor->mExitAddress) );

  //Эпилог
  gOperatorContextExit( prog, svFor );
  }




//Создать локальные переменные при входе в блок
void SrVpuCompiler::gOperatorContextEnter(SrProgramm *prog, SrOperatorContext *context)
  {
  //Последовательно для всех переменных выполняем создание
  for( SrVariable *var : context->mVarLocal.mList ) {

    if( var->mType->isObject() ) {
      //Для переменной типа Объект выполнить создание
      prog->addCode2Param8( VPC3_OBJ_BUILD, var->mAddress, var->mType->objectId(), context->mMark );
      codePrint( QString("VPC3_OBJ_BUILD %1 [%2] objectId=%3 [%4]").arg(var->mAddress).arg(var->mName).arg(var->mType->mName).arg(var->mType->objectId()) );
      }

    //Проверить наличие инициализационного выражения
    if( var->mInit ) {
      //Есть инициализационное выражение
      prog->addCodeParam8( VPC2_PUSH_B_OFFSET, var->mAddress, var->mMarkDefine );
      codePrint( QString("VPC2_PUSH_B_OFFSET ") + var->mName + QString("<%1>").arg( var->mAddress ) );
      //Теперь значение для сохранения
      gValue( prog, var->mInit, true, false );
      //Теперь сохранение
      if( var->mType->canAssign(var->mInit->mType) ) {
        if( var->mType->isObject() ) {
          prog->addCode( VPC1_OBJ_POP, var->mMarkDefine );
          codePrint( QString("VPC1_OBJ_POP") );
          }
        else {
          prog->addCode( VPC1_POP, var->mMarkDefine );
          codePrint( QString("VPC1_POP") );
          }
        }
      else
        errorInLine( QObject::tr("Error. Can't assign %1 to %2").arg(var->mInit->mType->mName).arg(var->mType->mName), var->mMarkDefine );
      }

    }

  }





//Эпилог контекста. Уничтожить локальные переменные
void SrVpuCompiler::gOperatorContextExit(SrProgramm *prog, SrOperatorContext *context)
  {
  //Последовательно для всех переменных выполняем уничтожение
  for( SrVariable *var : context->mVarLocal.mList ) {

    if( var->mType->isObject() ) {
      //Для переменной типа Объект выполнить уничтожение
      prog->addCodeParam8( VPC2_OBJ_CLEAR, var->mAddress, context->mMark );
      codePrint( QString("VPC2_OBJ_CLEAR %1 [%2] objectId=%3 [%4]").arg(var->mAddress).arg(var->mName).arg(var->mType->mName).arg(var->mType->objectId()) );
      }
    }

  }






//Пройти по иерархии вложенных операторов, для каждого попавшегося контекста
//уничтожаем переменные
void SrVpuCompiler::gOperatorTreeContextExit(SrProgramm *prog, SrOperator *op)
  {
  if( op == 0 ) return;
  SrOperatorContext *context = dynamic_cast<SrOperatorContext*>(op);
  if( context )
    gOperatorContextExit( prog, context );
  gOperatorTreeContextExit( prog, op->mParent );
  }







void SrVpuCompiler::gPushConst(SrProgramm *prog, int val, const SrMark &mark)
  {
  switch( val ) {
    case 0 :
      prog->addCode( VPC1_PUSH_0, mark );
      codePrint( QString("VPC1_PUSH_0") );
      break;
    case 1 :
      prog->addCode( VPC1_PUSH_1, mark );
      codePrint( QString("VPC1_PUSH_1") );
      break;
    case 2 :
      prog->addCode( VPC1_PUSH_2, mark );
      codePrint( QString("VPC1_PUSH_2") );
      break;
    default :
      if( val >= -128 && val <= 127 ) {
        prog->addCodeParam8( VPC2_PUSH_CONST, val, mark );
        codePrint( QString("VPC2_PUSH_CONST %1").arg(val) );
        }
      else if( val >= -32768 && val <= 32767 ) {
        prog->addCodeParam16( VPC3_PUSH_CONST, val, mark );
        codePrint( QString("VPC3_PUSH_CONST %1").arg(val) );
        }
      else if( val >= -8388608 && val <= 8388607 ) {
        prog->addCodeParam24( VPC4_PUSH_CONST, val, mark );
        codePrint( QString("VPC4_PUSH_CONST %1").arg(val) );
        }
      else {
        prog->addCodeParam32( VPC5_PUSH_CONST, val, mark );
        codePrint( QString("VPC5_PUSH_CONST %1").arg(val) );
        }
    }
  }




void SrVpuCompiler::gStack(SrProgramm *prog, int offset, const SrMark &mark)
  {
  switch( offset ) {
    case 0 : break;
    case -1 :
      prog->addCode( VPC1_STACK_DN1, mark );
      codePrint( "VPC1_STACK_DN1");
      break;
    case -2 :
      prog->addCode( VPC1_STACK_DN2, mark );
      codePrint( "VPC1_STACK_DN2");
      break;

    case 1 :
      prog->addCode( VPC1_STACK_UP1, mark );
      codePrint( "VPC1_STACK_UP1");
      break;
    case 2 :
      prog->addCode( VPC1_STACK_UP2, mark );
      codePrint( "VPC1_STACK_UP2");
      break;
    case 3 :
      prog->addCode( VPC1_STACK_UP3, mark );
      codePrint( "VPC1_STACK_UP3");
      break;
    case 4 :
      prog->addCode( VPC1_STACK_UP4, mark );
      codePrint( "VPC1_STACK_UP4");
      break;

    default :
      if( offset <= 127 && offset > -127 ) {
        prog->addCodeParam8( VPC2_STACK, offset, mark );
        codePrint( QString("VPC2_STACK ") + QString::number(offset) );
        }
      else {
        prog->addCodeParam16( VPC3_STACK, offset, mark );
        codePrint( QString("VPC3_STACK ") + QString::number(offset) );
        }
    }

  }

void SrVpuCompiler::gLoad(SrProgramm *prog, SrValue *val)
  {
  if( val->mType->isObject() ) {
    prog->addCode( VPC1_OBJ_LOAD, val->mMark );
    codePrint( QString("VPC1_OBJ_LOAD") );
    }
  else {
    prog->addCode( VPC1_LOAD, val->mMark );
    codePrint( QString("VPC1_LOAD") );
    }
  }




void SrVpuCompiler::codePrint(const QString code)
  {
  if( mList )
    (*mList) << code << '\n';
  }








