/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Генератор для VPU
*/

#ifndef TVPUCOMPILER_H
#define TVPUCOMPILER_H

#include "SrCompiler.h"
//#include "Vpu/SvVpuCore.h"
#include "SrHost/SrProgramm.h"
#include <QMap>

namespace SrCompiler6 {

  class SrVpuCompiler : public SrCompiler
    {
      QTextStream   *mList;     //!< Поток листинга
    public:
      SrVpuCompiler();
      ~SrVpuCompiler();

      //! Построить программу
      SrProgrammPtr make(const QString prjPath, const QString &mainScript );

      void pass(SrProgramm *prog);

      void gOperator( SrProgramm *prog, SrOperator *op );
      void gOperatorReturn( SrProgramm *prog, SrOperatorReturn *svReturn );
      void gOperatorCatch( SrProgramm *prog, SrOperatorCatch *svCatch );
      void gOperatorThrow( SrProgramm *prog, SrOperatorThrow *svThrow );
      void gOperatorBlock( SrProgramm *prog, SrOperatorBlock *svBlock );
      void gOperatorExpression(SrProgramm *prog, SrOperatorExpression *svExpression );
      void gOperatorIf( SrProgramm *prog, SrOperatorIf *svIf );
      void gOperatorBreak( SrProgramm *prog, SrOperatorBreak *svBreak );
      void gOperatorContinue( SrProgramm *prog, SrOperatorContinue *svContinue );
      void gOperatorWhile(SrProgramm *prog, SrOperatorWhile *svWhile );
      void gOperatorDoWhile( SrProgramm *prog, SrOperatorDoWhile *svDoWhile );
      void gOperatorFor( SrProgramm *prog, SrOperatorFor *svFor );
      void gOperatorContextEnter(SrProgramm *prog, SrOperatorContext *context );
      void gOperatorContextExit( SrProgramm *prog, SrOperatorContext *context );
      void gOperatorTreeContextExit( SrProgramm *prog, SrOperator *op );

      void gValue( SrProgramm *prog, SrValue *val, bool keepValue, bool address );
      void gPushConst(SrProgramm *prog, int val , const SrMark &mark );
      void gStack( SrProgramm *prog, int offset, const SrMark &mark );
      void gLoad( SrProgramm *prog, SrValue *val );

      void codePrint( const QString code );

      //void gvvRemoveTop( SrProgramm *prog, int count = 1 );
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
      void gvvUnary( SrProgramm *prog, SrValueUnary *unary, bool keepValue, bool address, SrVpuCode code, const QString codeList );

      void gvvNot(SrProgramm *prog, SrValueBitNot *vnot, bool keepValue, bool address );
      void gvvLogNot(SrProgramm *prog, SrValueLogNot *logNot, bool keepValue, bool address );
      void gvvNeg(SrProgramm *prog, SrValueNeg *neg, bool keepValue, bool address );

      void gvvStore(SrProgramm *prog, SrValueStore *store, bool keepValue, bool address );
      void gvvArrayCell(SrProgramm *prog, SrValueArrayCell *array, bool keepValue, bool address );
      void gvvBinary( SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList );
      void gvvBinaryStore( SrProgramm *prog, SrValueBinary *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList );
      void gvvBinaryLong( SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address, SrVpuCode code, const QString codeList );
      void gvvLogAnd( SrProgramm *prog, SrValueLogAnd *binary, bool keepValue, bool address );
      void gvvLogOr( SrProgramm *prog, SrValueLogOr *binary, bool keepValue, bool address );
      void gvvComma( SrProgramm *prog, SrValueBinaryLong *binary, bool keepValue, bool address );
      void gvvCondition( SrProgramm *prog, SrValueCondition *condition, bool keepValue, bool address );
      void gvvCall( SrProgramm *prog, SrValueCall *call, bool keepValue, bool address );

    };

}


#endif // TVPUCOMPILER_H
