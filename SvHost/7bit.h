#ifndef _7BIT_H
#define _7BIT_H

void w32to7( unsigned addr, int val, char d[] );

void r32to7( unsigned addr, char d[] );

unsigned parse7to32( char d[], int *dval );

void w16to7( unsigned addr, short val, char d[] );

void r16to7( unsigned addr, char d[] );

unsigned parse7to16( char d[], short *dval );

void w8to7( unsigned addr, char val, char d[] );

void r8to7( unsigned addr, char d[] );

unsigned parse7to8( char d[], char *dval );

#endif // _7BIT_H
