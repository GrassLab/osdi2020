#ifndef __LIB_STRING_H
#define __LIB_STRING_H

#include "type.h"

int strcmp ( const char * s1, const char * s2 );
void strset ( char * s1, int c, int size );
int strlen ( const char * s );
char * strcpy ( char * destination, const char * source );
char * strncpy ( char * destination, const char * source, int num );
const char * strchr ( const char * str, int character );
char * itoa ( int value, char * str, int base );
int atoi ( const char * str );
void ftoa ( double n, char * res, int afterpoint );
void reverse ( char * s );

#endif