#include "string.h"

int strcmp(const char* s1, const char* s2){
	for (; *s1 == *s2 ; s1++, s2++){
		if (*s1 == '\0')
			return 0;
	}
	return *s1 - *s2;
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

/**
 * Transfer a binary value to hexadecimal string
 */
void bin2hex(unsigned int num, char buff[11]) {
    unsigned int n;
    int c,i;
    for(i=2,c=28; c>=0; i++,c-=4) {
        // get highest tetrad
        n=(num>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
		buff[i]=(char)n;
    }
	buff[0]='0';
	buff[1]='x';
	buff[8]='\0';
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