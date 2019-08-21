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
#ifndef SVVPUCOMPILER_H
#define SVVPUCOMPILER_H

#include "SvCompiler.h"
#include "SvHost/SvProgramm.h"

#include <QMap>

namespace SvCompiler6 {

  /*!
     \brief The SvVpuCompiler class Данный класс представляет собой генератор
     байт-кода для скриптового языка rc++, который разбирается базовым классом
     SvCompiler
   */
  class SvVpuCompiler : public SvCompiler
    {
      QTextStream   *mList;     //!< Поток листинга
    public:
      SvVpuCompiler();
      ~SvVpuCompiler();

      /*!
         \brief make Построить программу. Компиляция и генерация кода
         \param prjPath Путь к каталогу со скриптами
         \param mainScript Имя файла главного скрипта
         \return Указатель со сгенерированной программой
       */
      SvProgrammPtr make(const QString prjPath, const QString &mainScript );

      static SvProgrammPtr build( const QString prjPath, const QString &mainScript );

    protected:

      //Добавить переменную в таблицу переменных генерируемой программы
      void addVariable(SvProgramm *prog, const QString prefix, SvVariable *var, int startAddress);

      //Один проход генерации
      void pass(SvProgramm *prog);

      //Генерация операторов
      void gOperator( SvProgramm *prog, SvOperator *op );
      void gOperatorReturn( SvProgramm *prog, SvOperatorReturn *svReturn );
      void gOperatorBlock( SvProgramm *prog, SvOperatorBlock *svBlock );
      void gOperatorExpression(SvProgramm *prog, SvOperatorExpression *svExpression );
      void gOperatorIf( SvProgramm *prog, SvOperatorIf *svIf );
      void gOperatorBreak( SvProgramm *prog, SvOperatorBreak *svBreak );
      void gOperatorContinue( SvProgramm *prog, SvOperatorContinue *svContinue );
      void gOperatorWhile(SvProgramm *prog, SvOperatorWhile *svWhile );
      void gOperatorDoWhile( SvProgramm *prog, SvOperatorDoWhile *svDoWhile );
      void gOperatorFor( SvProgramm *prog, SvOperatorFor *svFor );

      //Генерация выражений
      void gValue( SvProgramm *prog, SvValue *val, bool keepValue, bool address );
      void gValueCatchFun(SvProgramm *prog, SvValueCatchFun *svCatch );
      void gValueThrowFun( SvProgramm *prog, SvValueThrowFun *svThrow );
      void gPushConst(SvProgramm *prog, int val , const SvMark &mark );
      void gStack( SvProgramm *prog, int offset, const SvMark &mark );
      void gLoad( SvProgramm *prog, SvValue *val );

      void gvvVariable( SvProgramm *prog, SvValueVariable *var, bool keepValue, bool address );
      void gvvFunction( SvProgramm *prog, SvValueFunction *fun, bool keepValue, bool address );
      void gvvConstInt( SvProgramm *prog, SvValueConstInt *cnst, bool keepValue, bool address );
      void gvvConstString( SvProgramm *prog, SvValueConstString *cnst, bool keepValue, bool address );
      void gvvPointer( SvProgramm *prog, SvValuePointer *ptr, bool keepValue, bool address );
      void gvvMemberVariable( SvProgramm *prog, SvValueMemberVariable *var, bool keepValue, bool address );
      void gvvAddress( SvProgramm *prog, SvValueAddress *addr, bool keepValue, bool address );
      void gvvPredInc( SvProgramm *prog, SvValuePredInc *inc, bool keepValue, bool address );
      void gvvPredDec( SvProgramm *prog, SvValuePredDec *dec, bool keepValue, bool address );
      void gvvPostInc( SvProgramm *prog, SvValuePostInc *inc, bool keepValue, bool address );
      void gvvPostDec(SvProgramm *prog, SvValuePostDec *dec, bool keepValue, bool address );
      void gvvUnary( SvProgramm *prog, SvValueUnary *unary, bool keepValue, bool address, SvVmCode code, const QString codeList );

      void gvvNot(SvProgramm *prog, SvValueBitNot *vnot, bool keepValue, bool address );
      void gvvLogNot(SvProgramm *prog, SvValueLogNot *logNot, bool keepValue, bool address );
      void gvvNeg(SvProgramm *prog, SvValueNeg *neg, bool keepValue, bool address );

      void gvvStore(SvProgramm *prog, SvValueStore *store, bool keepValue, bool address );
      void gvvArrayCell(SvProgramm *prog, SvValueArrayCell *array, bool keepValue, bool address );
      void gvvBinary( SvProgramm *prog, SvValueBinary *binary, bool keepValue, bool address, SvVmCode code, const QString codeList );
      void gvvBinaryStore( SvProgramm *prog, SvValueBinary *binary, bool keepValue, bool address, SvVmCode code, const QString codeList );
      void gvvBinaryLong( SvProgramm *prog, SvValueBinaryLong *binary, bool keepValue, bool address, SvVmCode code, const QString codeList );
      void gvvLogAnd( SvProgramm *prog, SvValueLogAnd *binary, bool keepValue, bool address );
      void gvvLogOr( SvProgramm *prog, SvValueLogOr *binary, bool keepValue, bool address );
      void gvvComma( SvProgramm *prog, SvValueBinaryLong *binary, bool keepValue, bool address );
      void gvvCondition( SvProgramm *prog, SvValueCondition *condition, bool keepValue, bool address );
      void gvvCall( SvProgramm *prog, SvValueCall *call, bool keepValue, bool address );

      //Вывод в листинг
      //! Если задан файл листинга, то выводит в него заданную строку
      void codePrint( const QString str );

      //! Если задан файл листинга, то выводит в него строку и символ новой строки
      void codePrintEoln( const QString str );

    };

}


#endif // SVVPUCOMPILER_H
