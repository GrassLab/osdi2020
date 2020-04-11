#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include "meta_macro.h"

int string_cmp(const char * string_a, const char * string_b, const int length);
void string_reverse_sequence(const char * src, char * dst, int size);
void string_strip(char * string, char c);
void string_concat(char * a, const char * b);
int string_length(const char * s);
void string_longlong_to_char(char * string, const long long i);
void string_ulonglong_to_hex_char(char * string, const unsigned long long i);
void string_float_to_char(char * string, const float f);
unsigned long long string_hex_char_to_longlong(char * string);

#endif

