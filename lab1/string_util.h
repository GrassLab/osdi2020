#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#ifdef DEBUG
#include "miniuart.h"
#endif

int string_cmp(const char * string_a, const char * string_b);
void string_strip_newline(char * string);

#endif

