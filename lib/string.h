#ifndef _LIB_STRING_H
#define _LIB_STRING_H 

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned long n);
void *memset(void *s, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);
char *strtok(char *s, const char delim);

#endif//_LIB_STRING_H