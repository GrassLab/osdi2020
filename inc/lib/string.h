#ifndef _LIB_STRING_H
#define _LIB_STRING_H 

unsigned int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strrchr(const char *s, char c);
char *strtok(char *s, const char delim);
void *memset(void *s, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);

#endif//_LIB_STRING_H