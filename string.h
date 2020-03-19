#ifndef _STRING_H_
#define _STRING_H_
#include "common.h"

int my_strcmp(const char* s1, const char* s2);

char *my_strcat(char *dest, const char *src);

void *my_memset(void *str, int c, size_t n);

void reverse(char str[], int length);
  
char* itoa(int num, char* str, int base);

void ftoa(float n, char* res, int afterpoint);

#endif
