#include "string.h"

int strcmp(const char *s1, const char *s2){
    for(;*s1 && *s1 == *s2; s1++, s2++);
    return *s1 - *s2;
}

char *strcpy(char *dest, const char *src){
    char *odest = dest;
    while( (*dest++ = *src++) );
    return odest;
}

char *strncpy(char *dest, const char *src, size_t n){
    char *odest = dest;
    while( n-- && (*dest++ = *src++) );
    return odest;
}

void *memset(void *s, int c, size_t n)
{
    char *xs = s;
    while (n--)
        *xs++ = c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *tmp = dest;
    const char *s = src;
    while (n--)
        *tmp++ = *s++;
    return dest;
}