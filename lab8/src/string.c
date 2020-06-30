#include "string.h"
#include "config.h"

void *_memset(void *dst, int val, int count){
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}


void *_memcpy(const void *from, void *dst, int count){
	const char *f = from;
	char *d = dst;

	while (count--)
		*d++ = *f++;

	return dst;
}


void *_copy_stack(const void *from, void *dst, int count){
	const char *f = from;
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

char *strcpy(char *dest, const char *src){
    char *odest = dest;
    while( (*dest++ = *src++) );
    return odest;
}

char *strtok(char *s, const char delim){
    static char *pos;
    char* ret;
	// s is not null, use new data
	// if not, use static pos(old data, new split location)
    if(s) pos = s;
    
    if(*pos == '\0') return 0;
    // skip continuous the same delim
    while(*pos == delim){
        pos ++;
    }

	//ret is head
	//pos is head of next split
    ret = pos;
    while(*pos != delim && *pos != '\0'){
        pos ++;
    }
    if (*pos != '\0'){
        *pos = '\0';
		//pos is head of next split
        pos ++;
    }
    return ret;
}