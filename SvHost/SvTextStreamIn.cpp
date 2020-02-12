#include "SvTextStreamIn.h"

SvTextStreamIn::SvTextStreamIn(const char *buf) :
  mBuffer(buf),
  mPtr(1),
  mUsedBits(0)
  {

  }


//Базовая функция извлечения
//Сначала получаем младшую половину из одного байта
//затем старшую половину - из следующего байта
int SvTextStreamIn::getInt8()
  {
  //Получаем первую порцию данных
  int val = (mBuffer[mPtr++] & 0x7f) >> mUsedBits;
  //Количество считанных битов
  int bits = 7 - mUsedBits;
  //Оставшиеся биты
  val |= ((mBuffer[mPtr] & 0x7f) << bits) & 0xff;
  //Сколько было оставшихся битов (забрали из текущего байта)
  mUsedBits = 8 - bits;
  if( mUsedBits == 7 ) {
    //Из текущего байта забрали все
    //переходим к следующему байту
    mPtr++;
    mUsedBits = 0;
    }
  return val;
  }




int SvTextStreamIn::getInt16()
  {
  int val = getInt8();
  val |= getInt8() << 8;
  return val;
  }




int SvTextStreamIn::getInt32()
  {
  int val = getInt8();
  val |= getInt8() << 8;
  val |= getInt8() << 16;
  val |= getInt8() << 24;
  return val;
  }



void SvTextStreamIn::getBlock(char *dest, int size)
  {
  for( int i = 0; i < size; i++ )
    dest[i] = getInt8();
//  dest[size] = 0;
  }
