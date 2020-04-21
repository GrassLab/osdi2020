#ifndef __STRING_H_
#define __STRING_H_

#include "types.h"

size_t strlen(const char *s);

int strcmp(const char *s1, const char *s2);

void reverse(char *s);

void itoa(int n, char *s);

void strcpy(char *dest, const char *src);

void memcpy(void *dest, void *src, size_t n);

#endif
