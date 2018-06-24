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
*/
#ifndef SRVPUCOMPILER_H
#define SRVPUCOMPILER_H

#include "SrCompiler.h"
#include "SrHost/SrProgramm.h"

#include <QMap>

namespace SrCompiler6 {

  /*!
     \brief The SrVpuCompiler class Данный класс представляет собой генератор
     байт-кода для скриптового языка rc++, который разбирается базовым классом
     SrCompiler
   */
  class SrVpuCompiler : public SrCompiler
    {
      QTextStream   *mList;     //!< Поток листинга
    public:
      SrVpuCompiler();
      ~SrVpuCompiler();

      /*!
         \brief make Построить программу. Компиляция и генерация кода
         \param prjPath Путь к каталогу со скриптами
         \param mainScript Имя файла главного скрипта
         \return Указатель со сгенерированной программой
       */
      SrProgrammPtr make(const QString prjPath, const QString &mainScript );

    protected:

      //Добавить переменную в таблицу переменных генерируемой программы
      void addVariable(SrProgramm *prog, const QString prefix, SrVariable *var, int startAddress);

      //Один проход генерации
      void pass(SrProgramm *prog);

      //Генерация операторов
      void gOperator( SrProgramm *prog, SrOperator *op );
      void gOperatorReturn( SrProgramm *prog, SrOperatorReturn *svReturn );
      void gOperatorBlock( SrProgramm *prog, SrOperatorBlock *svBlock );
      void gOperatorExpression(SrProgramm *prog, SrOperatorExpression *svExpression );
      void gOperatorIf( SrProgramm *prog, SrOperatorIf *svIf );
      void gOperatorBreak( SrProgramm *prog, SrOperatorBreak *svBreak );
      void gOperatorContinue( SrProgramm *prog, SrOperatorContinue *svContinue );
      void gOperatorWhile(SrProgramm *prog, SrOperatorWhile *svWhile );
      void gOperatorDoWhile( SrProgramm *prog, SrOperatorDoWhile *svDoWhile );
      void gOperatorFor( SrProgramm *prog, SrOperatorFor *svFor );
      void gOperatorContextEnter(SrProgramm *prog, SrOperatorContext *context );

      //Генерация выражений
      void gValue( SrProgramm *prog, SrValue *val, bool keepValue, bool address );
      void gValueCatchFun(SrProgramm *prog, SrValueCatchFun *svCatch );
      void gValueThrowFun( SrProgramm *prog, SrValueThrowFun *svThrow );
      void gPushConst(SrProgramm *prog, int val , const SrMark &mark , const QString label = QString() );
      void gStack( SrProgramm *prog, int offset, const SrMark &mark );
      void gLoad( SrProgramm *prog, SrValue *val );

      void gvvVariable( SrProgramm *prog, SrValueVariable *var, bool keepValue, bool address );
      void gvvFunction( SrProgramm *prog, SrValueFunction *fun, bool keepValue, bool address );
      void gvvConstInt( SrProgramm *prog, SrValueConstInt *cnst, bool keepValue, bool address );
      void gvvConstString( SrProgramm *prog, SrValueConstString *cnst, bool keepValue, bool address );
      void gvvPointer( SrProgramm *prog, SrValuePointer *ptr, bool keepValue, bool address );
      void gvvMemberVariable( SrProgramm *prog, SrValueMemberVariable *var, bool keepValue, bool address );
      void gvvAddress( SrProgramm *prog, SrValueAddress *addr, bool keepValue, bool address );
      void gvvPredInc( SrProgramm *prog, SrValuePredInc *inc, bool keepValue, bool address );
      void gvvPredDec( SrProgramm *prog, SrValuePredDec *dec, bool keepValue, bool address );
      void gvvPostInc( SrProgramm *prog, SrValuePostInc *inc, bool keepValue, bool address );
      void gvvPostDec(SrProgramm *prog, SrValuePostDec *dec, bool keepValue, bool address );
      void gvvUnary( SrProgramm *prog, SrValueUnary *unary, bool keepValue, bool address, SrVmCode code, const QString codeList );

      void gvvNot(SrProgramm *prog, SrValueBitNot *vnot, bool keepValue, bool address );
      void gvvLogNot(SrProgramm *prog, SrValueLogNot *logNot, bool keepValue, bool address );
      void gvvNeg(SrProgramm *prog, SrValueNeg *neg, bool keepValue, bool address );

      void gvvStore(SrProgramm *prog, SrValueStore *store, bool keepValue, bool address );
      void gvvArrayCell(SrProgramm *prog, SrValueArrayCell *array, bool keepValue, bool address );
      void gvvBinary( SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVmCode code, const QString codeList );
      void gvvBinaryStore( SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVmCode code, const QString codeList );
      void gvvBinaryLong( SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address, SrVmCode code, const QString codeList );
      void gvvLogAnd( SrProgramm *prog, SrValueLogAnd *binary, bool keepValue, bool address );
      void gvvLogOr( SrProgramm *prog, SrValueLogOr *binary, bool keepValue, bool address );
      void gvvComma( SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address );
      void gvvCondition( SrProgramm *prog, SrValueCondition *condition, bool keepValue, bool address );
      void gvvCall( SrProgramm *prog, SrValueCall *call, bool keepValue, bool address );

      //Вывод в листинг
      void codePrint( const QString code );

    };

}


#endif // SRVPUCOMPILER_H
