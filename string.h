#ifndef _STRING_H_
#define _STRING_H_
#include "common.h"

int my_strcmp(const char* s1, const char* s2);

void *my_memset(void *str, int c, size_t n);

void reverse(char str[], int length);
  
// Implementation of itoa() 
char* itoa(int num, char* str, int base);

void ftoa(float n, char* res, int afterpoint);

#endif
