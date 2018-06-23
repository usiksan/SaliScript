/*
  Project     "Script language reduced c++ (rc++)"
  SubProject  "Virtual machine"
  Author
    Alexander Sibilev
  Internet
    www.rc.saliLab.ru - project home site
    www.saliLab.ru
    www.saliLab.com

  Description
    Virtual machine types
*/

#ifndef SRVMTYPES
#define SRVMTYPES
#include <QString>

//Int value
typedef int                 SrVmInt;

//Float value
typedef float               SrVmFloat;

//One byte of programm byte-code
typedef const unsigned char SrVmCode;

//One unicode character [Один символ unicode]
typedef unsigned short      SrVmUnicode;


#endif // SRVMTYPES

