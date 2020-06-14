#ifndef STRING_H
#define STRING_H

#include "type.h"

int strcmp ( const char * s1, const char * s2 );
void strset ( char * s1, int c, int size );
int strlen ( const char * s );
void itoa ( int x, char str[], int d );
void ftoa ( float n, char * res, int afterpoint );
void itohex_str ( uint64_t d, int size, char * s );
void reverse ( char * s );

#endif