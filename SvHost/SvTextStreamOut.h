#ifndef SVTEXTSTREAMOUT_H
#define SVTEXTSTREAMOUT_H


class SvTextStreamOut
  {
    char  mBuffer[512];
    short mPtr;
    short mUsedBits;
  public:
    SvTextStreamOut();

    void addInt8( int val );
    void addInt16( int val );
    void addInt32( int val );
    void addBlock( const char *block, int size );
    void begin( char cmd );
    void end();

    const char *buffer() const;
  };

#endif // SVTEXTSTREAMOUT_H
