#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <stddef.h>

void stripString(char *str);
int compareString(const char *s1, const char *s2);
unsigned int getIntegerFromString(const char *str);
size_t strlen(const char *str);

#endif
