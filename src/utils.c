#include "utils.h"

int strcmp(const char* s1, const char* s2){
	for (; *s1 == *s2 ; s1++, s2++){
		if (*s1 == '\0')
			return 0;
	}
	return *s1 - *s2;
}

// TODO: need implement malloc for dynamic length array
// TODO: uding while version to eliminate leading zeros
void unitoa(unsigned num, char *buff, unsigned num_dig){
	unsigned int ASCII_BIAS = 48;
	buff[num_dig] = '\0';
	for(int i=num_dig-1; i>=0; i--){
		buff[i] = (char)(num%10 + ASCII_BIAS);
		num = num / 10;
	}
}

/**
 * Transfer a binary value to hexadecimal string
 */
void bin2hex(unsigned int num, char buff[4]) {
    unsigned int n;
    int c,i;
    for(i=0,c=28; c>=0; i++,c-=4) {
        // get highest tetrad
        n=(num>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
		buff[i]=(char)n;
    }
}