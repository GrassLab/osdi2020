#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#ifdef DEBUG
#include "miniuart.h"
#endif

#include "meta_macro.h"

int string_cmp(const char * string_a, const char * string_b);
void string_reverse_sequence(const char * src, char * dst, int size);
void string_strip(char * string, char c);
void string_concat(char * a, const char * b);
void string_longlong_to_char(char * string, const long long i);
void string_float_to_char(char * string, const float f);

#endif

