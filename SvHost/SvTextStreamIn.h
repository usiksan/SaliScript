#ifndef SVTEXTSTREAMIN_H
#define SVTEXTSTREAMIN_H


class SvTextStreamIn
  {
    const char *mBuffer;
    int         mPtr;
    int         mUsedBits;
  public:
    SvTextStreamIn( const char *buf );

    char  getCmd() { return mBuffer[0]; }
    int   getInt8();
    int   getInt16();
    int   getInt32();
    void  getBlock( char *dest, int size );
  };

#endif // SVTEXTSTREAMIN_H
