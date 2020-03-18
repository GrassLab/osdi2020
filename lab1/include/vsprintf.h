#ifndef _VSPRINTF_INCLUDE_H_
#define _VSPRINTF_INCLUDE_H_
char *int_to_str(char *str, int num, int base);
char *float_to_str(char *str, float num, int precision);
char *vsprintf(char *buf, const char *fmt, __builtin_va_list args);
#endif