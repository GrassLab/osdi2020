#include "string.h"
#include "config.h"

void *_memset(void *dst, int val, int count){
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}


void *_memcpy(const void *from, void *dst, int count){
	char *f = from;
	char *d = dst;

	while (count--)
		*d++ = *f++;

	return dst;
}


void *_copy_stack(const void *from, void *dst, int count){
	char *f = from;
	char *d = dst;

	while (count--)
		*d-- = *f--;

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

int _strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void _unsign_arr_to_digit(unsigned num, char* buf, unsigned len){
	for(int i=len-1; i>=0; i--){
		buf[i] = (char)(num%10 + '0');
		num /= 10;
	}
}

