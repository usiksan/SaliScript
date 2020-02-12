#include "SvTextStreamOut.h"

SvTextStreamOut::SvTextStreamOut()
  {

  }


void SvTextStreamOut::addInt8(int val)
  {
  //Добавляем биты в текущий байт
  mBuffer[mPtr++] |= (val << mUsedBits) | 0x80;

  //Количество битов добавленных выше
  int bits = 7 - mUsedBits;

  //Количество использованных битов в новом байте
  mUsedBits = 8 - bits;
  mBuffer[mPtr] = ((val & 0xff) >> bits) | 0x80;
  if( mUsedBits == 7 ) {
    mPtr++;
    mBuffer[mPtr] = 0;
    mUsedBits = 0;
    }
  }

void SvTextStreamOut::addInt16(int val)
  {
  addInt8(val);
  addInt8(val >> 8);
  }

void SvTextStreamOut::addInt32(int val)
  {
  addInt8(val);
  addInt8(val >> 8);
  addInt8(val >> 16);
  addInt8(val >> 24);
  }

void SvTextStreamOut::addBlock(const char *block, int size)
  {
  for( int i = 0; i < size; i++ )
    addInt8( block[i] );
  }

void SvTextStreamOut::begin(char cmd)
  {
  mUsedBits = 0;
  mPtr = 1;
  mBuffer[0] = cmd & 0x7f;
  mBuffer[1] = 0;
  }

void SvTextStreamOut::end()
  {
  if( mUsedBits ) mPtr++;
  mBuffer[mPtr++] = '\n';
  mBuffer[mPtr] = 0;
  }
