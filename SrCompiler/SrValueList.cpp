/*
Project "VPU and SML compiler v6"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  struct SvValueList - list of binding values
*/

#include "SrCompiler.h"

using namespace SrCompiler6;

QString SrValueList::listing()
  {
  QString str;
  for( int index = 0; index < mList.count(); index++ )
    if( mList.at(index) )
      str.append( QString("%1 : %2\n").arg(index).arg(mList.at(index)->listing()) );
    else
      str.append( QString("%1 : NULL\n").arg(index) );
  return str;
  }
