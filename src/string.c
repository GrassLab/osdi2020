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

char *strncpy(char *dest, const char *src, unsigned int n){
    char *odest = dest;
    while( n-- && (*dest++ = *src++) );
    return odest;
}

void unitoa(unsigned num, char *buff, unsigned num_dig){
	buff[num_dig] = '\0';
	for(int i=num_dig-1; i>=0; i--){
		buff[i] = (char)(num%10 + '0');
		num = num / 10;
	}
}

int atoi(char *ascii){
	int num = 0;
	char *c;
	for(c=ascii; *c!='\0'; c=++ascii){
			num *= 10;
			num += (*c - '0');
	}
	return num;
}

int hex2int(char *hex){
	int num = 0;
	char *c;
	for(c=hex+2; *c!='\0'; c++){
			num *= 16;
			num += *c>'9'?(*c-'A'+10):(*c-'0');
	}
	return num;
}