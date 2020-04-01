#include "string.h"
#include "config.h"

void *_memset(void *dst, int val, int count){
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}

void _print(char * c){
    uart_puts(c);
}

int _strlen(char * str){
    char *s;
    for (s = str; *s; ++s)
        ;
    return (s - str);
}

int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}