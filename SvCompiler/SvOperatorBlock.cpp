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
    Пико компилятор скриптового языка rc++

    Реализация класса SvOperatorBlock - блок операторов
*/


#include "SvCompiler.h"

using namespace SvCompiler6;

SvOperatorBlock::SvOperatorBlock(const SvMark &mark, const QString remark, SvOperatorContext *parent) :
  SvOperatorContext( mark, remark, parent )
  {

  }

SvOperatorBlock::~SvOperatorBlock()
  {
  qDeleteAll(mList);
  }

QString SvOperatorBlock::listing(int level)
  {
  level++;
  //Пролог
  QString str( padding(level) );
  str.append("{\n");
  //Переменные блока
  str.append( mVarLocal.listing(level) );
  for( SvOperator *op : mList )
    str.append( op->listing(level) );
  //Эпилог
  str.append( padding(level) ).append("}\n");
  return str;
  }
