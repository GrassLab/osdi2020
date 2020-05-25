#ifndef USER_LIB_STRING_H_
#define USER_LIB_STRING_H_

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);
char *strtrim(char *s);

#endif // USER_LIB_STRING_H_
