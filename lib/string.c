#include "string.h"
#include "printf.h"

int strcmp(const char *s1, const char *s2){
    for(;*s1 && *s1 == *s2; s1++, s2++);
    return *s1 - *s2;
}

char *strcpy(char *dest, const char *src){
    char *odest = dest;
    while( (*dest++ = *src++) );
    return odest;
}

char *strncpy(char *dest, const char *src, unsigned long n){
    char *odest = dest;
    while( n-- && (*dest++ = *src++) );
    return odest;
}

void *memset(void *s, int c, unsigned long n)
{
    char *xs = s;
    while (n--)
        *xs++ = c;
    return s;
}

void *memcpy(void *dest, const void *src, unsigned long n)
{
    #ifdef __DEBUG
    printf("[memcpy] dest: 0x%X\tsrc: 0x%X\tsize: %d\n",dest, src, n);
    #endif//__DEBUG
    char *tmp = dest;
    const char *s = src;
    while (n--)
        *tmp++ = *s++;
    return dest;
}

char *strtok(char *s, const char delim){
    static char *pos;
    char* ret;
    if(s) pos = s;
    
    if(*pos == '\0') return 0;
    // skip leading
    while(*pos == delim){
        pos ++;
    }

    ret = pos;
    while(*pos != delim && *pos != '\0'){
        pos ++;
    }
    if (*pos != '\0'){
        *pos = '\0';
        pos ++;
    }
    return ret;
}