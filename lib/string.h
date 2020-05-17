#ifndef _LIB_STRING_H
#define _LIB_STRING_H 

typedef unsigned int size_t;
typedef int ssize_t;

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
#endif//_LIB_STRING_H