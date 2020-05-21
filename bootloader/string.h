#ifndef _STRING_H_
#define _STRING_H_
#include "common.h"

int strcmp(const char* s1, const char* s2);

char *my_strcat(char *dest, const char *src);

void *memset(void *str, int c, size_t n);

void * memcpy (void *dest, const void *src, size_t len);

#endif
