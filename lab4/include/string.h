#ifndef __STRING_H__
#define __STRING_H__
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int c);
unsigned int strlen(const char *s);
void memcpy(void *dest, const void *src, unsigned long num);
void *memset(void *s, int c, unsigned long n);
#endif
