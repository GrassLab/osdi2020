#ifndef _UTILS_H
#define _UTILS_H

#include "type.h"

size_t round(size_t src);
size_t copynstr(const char *src, char *dst, size_t len);
bool strcmp(const char *s1, const char *s2);
void printInt(uint32_t i);
void printFloat(double f);
uint32_t atoi(char *s);

#endif