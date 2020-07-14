#ifndef _STRING_H
#define _STRING_H

typedef unsigned long size_t;

int strcmp(const char *str1, const char *str2);
int strcpy(char *dest, const char *src);
int strncpy(char *dest, const char *src,int n);
int strcpy_delim(char *dest, const char *src,int len,const char delim);
int strtolower(char *str);
int strlen ( const char * s );
char *strcat (char *dest, const char *src);

#endif  /*_STRING_H */
