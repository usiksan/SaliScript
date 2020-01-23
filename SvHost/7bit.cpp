

void w32to7( unsigned addr, int val, char d[] ) {
  //команда 0x40
  d[0] = 0x40 | ((addr >> 2) & 0x3f);
  d[1] = 0x80 | ((addr >> 8) & 0x7f);
  d[2] = 0x80 | ((addr >> 15) & 0x7f);
  d[3] = 0x80 | ((addr >> 22) & 0x7f);
  d[4] = 0x80 | ((addr >> 29) & 0x7);
  d[4] |= (val << 3) & 0x78;
  d[5] = 0x80 | ((val >> 4) & 0x7f);
  d[6] = 0x80 | ((val >> 11) & 0x7f);
  d[7] = 0x80 | ((val >> 18) & 0x7f);
  d[8] = 0x80 | ((val >> 25) & 0x7f);
  d[9] = '\n';
  d[10]= 0;
  }

void r32to7( unsigned addr, char d[] ) {
  w32to7( addr, 0, d );
  d[5] = '\n';
  d[6] = 0;
  }

unsigned parse7to32( char d[], int *dval ) {
  unsigned addr = 0;
  int val = 0;
  addr = d[4] & 0x7;
  addr <<= 3;
  addr |= d[3] & 0x7f;
  addr <<= 7;
  addr |= d[2] & 0x7f;
  addr <<= 7;
  addr |= d[1] & 0x7f;
  addr <<= 6;
  addr |= d[0] & 0x3f;
  addr <<= 2;
  if( d[5] & 0x80 ) {
    //Команда записи
    val = d[8] & 0x7f;
    val <<= 7;
    val |= d[7] & 0x7f;
    val <<= 7;
    val |= d[6] & 0x7f;
    val <<= 7;
    val |= d[5] & 0x7f;
    val <<= 4;
    val |= (d[4] >> 3) & 0xf;
    //Запрос на запись
    *dval = val;
    }
  return addr;
  }

void w16to7( unsigned addr, short val, char d[] ) {
  //команда 0x80
  d[0] = 0x80 | ((addr >> 1) & 0x3f);
  d[1] = 0x80 | ((addr >> 7) & 0x7f);
  d[2] = 0x80 | ((addr >> 14) & 0x7f);
  d[3] = 0x80 | ((addr >> 21) & 0x7f);
  d[4] = 0x80 | ((addr >> 28) & 0xf);
  d[4] |= (val << 3) & 0x70;
  d[5] = 0x80 | ((val >> 3) & 0x7f);
  d[6] = 0x80 | ((val >> 10) & 0x3f);
  d[7] = '\n';
  d[8] = 0;
  }

void r16to7( unsigned addr, char d[] ) {
  w16to7( addr, 0, d );
  d[5] = '\n';
  d[6] = 0;
  }


void w8to7( unsigned addr, char val, char d[] ) {
  //команда 0xc0
  d[0] = 0xc0 | ((addr >> 1) & 0x3f);
  d[1] = 0x80 | ((addr >> 7) & 0x7f);
  d[2] = 0x80 | ((addr >> 14) & 0x7f);
  d[3] = 0x80 | ((addr >> 21) & 0x7f);
  d[4] = 0x80 | ((addr >> 28) & 0xf);
  d[4] |= (val << 3) & 0x70;
  d[5] = 0x80 | ((val >> 3) & 0xf);
  d[6] = '\n';
  d[7] = 0;
  }

void r8to7( unsigned addr, char d[] ) {
  w8to7( addr, 0, d );
  d[5] = '\n';
  d[6] = 0;
  }
