/*
Проект "Компилятор"
Содержание
  Реализация класса TCompiler часть 4 (таблица символов)
  Функции:
            //Коментарий
            void     AddAsmRemark( CPChar sour, bool bAsm = false );          //Добавить коментарий, ассемблерный код
            //Таблица символов
            TSymbol* FindSymbol( CPChar name );
            int      AddSymbol( TSymbol &sym );
            void     DecLevel();
            //Таблица псевдокода
            void     AddTriac( TTriac &t, TOperand *op );
            void     AddTriac( TOperation op, TOperandType opt1 = totInt, TUINT32 opv1 = 0 );
            void     PostLabel( int label );
            int      GetTriacPos() const { return mIndirectTable.GetNumber(); }
            int      InsertTriac( int before, TTriac &t, TOperand *op );
            void     MoveTriac( int from, int to, int count );
            //Таблица типов
            TType*   AllocType( TType *subType = 0 );
            bool     CompareType( TType *type1, TType *type2 );
*/
#include "SrCompiler.h"

using namespace SrCompiler6;

//==============================================================================
//---------------------- TRemarkTable ------------------------------------------
void
SrCompiler::AddAsmRemark( const QString &sour ) {
  //Создать триак с коментарием
//  AddOperation( toRemark, mTypeVoid, mRemarkTable.count(), 0 );
  //Скопировать коментарий в таблицу коментариев
  mRemarkTable.append( sour );
  }




bool SrCompiler::isGlobalSymbol(const QString &name)
  {
  //Проверить среди типов
  if( mTypeList.isPresent(name) )
    return true;
  //Проверить среди переменных
  if( mVarGlobal.isPresent(name) )
    return true;
  //Проверить среди функций
  return mFunGlobal.isPresent(name);
  }























