/*
            Virtual Processor Unit  (VPU5)
  Author
    Alexander Sibilev
  Internet
     www.saliLab.ru
     www.saliLab.com

  Description
    Утилиты общего назначения
*/
#ifndef SVUTILS_H
#define SVUTILS_H

inline int svIMin( int i1, int i2 ) { return i1 < i2 ? i1 : i2; }
inline int svIMax( int i1, int i2 ) { return i1 > i2 ? i1 : i2; }
inline int svIAbs( int i1 ) { return i1 < 0 ? -i1 : i1; }

//! Ограничение (включительно, т.е. значения limMin и limMax - допустимы)
inline int svILimit( int val, int limMin, int limMax ) { if( val < limMin ) return limMin; if( val > limMax ) return limMax; return val; }

//! Заворачивание, значение limMin - допустимо, меньшие значения заворачиваются на limMax - 1, таким образом limMax не входит в диапазон
inline int svIWrap( int val, int limMin, int limMax ) { if( val < limMin ) return limMax - 1; if( val >= limMax ) return limMin; return val; }

//! Чтение невыравненного целого старшими вперед
inline int svIRead16( const unsigned char *src ) { return (signed)((src[0] << 24) | (src[1] << 16)) >> 16; }
inline int svIRead24( const unsigned char *src ) { return (signed)((src[0] << 24) | (src[1] << 16) | (src[2] << 8)) >> 8; }
inline int svIRead32( const unsigned char *src ) { return ((src[0] << 24) | (src[1] << 16) | (src[2] << 8) | (src[3])); }

//! Чтение невыравненного целого младшими вперед
inline int svIRead32lsb( const unsigned char *src ) { return ((src[0] << 0) | (src[1] << 8) | (src[2] << 16) | (src[3] << 24)); }

//! Запись невыравненного целого
inline void svIWrite16( unsigned char *dest, int val ) {
  dest[1] = (unsigned char)(val);
  dest[0] = (unsigned char)(val >> 8);
  }
inline void svIWrite24( unsigned char *dest, int val ) {
  dest[2] = (unsigned char)(val);
  dest[1] = (unsigned char)(val >> 8);
  dest[0] = (unsigned char)(val >> 16);
  }
inline void svIWrite32( unsigned char *dest, int val ) {
  dest[3] = (unsigned char)(val);
  dest[2] = (unsigned char)(val >> 8);
  dest[1] = (unsigned char)(val >> 16);
  dest[0] = (unsigned char)(val >> 24);
  }

inline unsigned svUMin( unsigned i1, unsigned i2 ) { return i1 < i2 ? i1 : i2; }
inline unsigned svUMax( unsigned i1, unsigned i2 ) { return i1 > i2 ? i1 : i2; }


#endif // SVUTILS_H
