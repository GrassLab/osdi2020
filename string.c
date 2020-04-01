#include "string.h"

void setstr(char* s, char c, int size) {
	for(int i=0; i<size; i++)
		s[i] = c;
}

int cmpstr(const char* a, const char* b) {
	while(*a) {
		if(*a != *b)
			break;
		a++;
		b++;
	}
	// return ASCII difference
	return *(const unsigned char*)a - *(const unsigned char*) b;
}

// copy string a to string b
void cpystr(char* a, char* b, int size) {
	for(int i=0; i < size; i++)
		a[i] = b[i];
}

// convert unsigned integer into string, decimal format
char* itoa(unsigned int val, char* str) {
	int i = 0;
	while(val) {
		str[i++] = val % 10 + '0';
		val /= 10;
	}
	if(i == 0) {
		str[0] = '0';
		str[1] = 0;
	}
	else
		str[i] = '\0';

	return revstr(str, 0, i-1);
}

// convert fractional part into string, decimal format
// fractional & denominator
char* ftoa(unsigned int frac, unsigned int deno, char* str) {
	int i = -1;
	while(frac && i < 9) {
		if(frac/deno == 0) {
			str[i++] = '0';
			frac *= 10;
		}
		else {
			str[i++] = frac/deno + '0';
			frac %= deno;
			frac *= 10;
		}
	}
	if(i == 0) {
		str[0] = '0';
		str[1] = 0;
	}
	else
		str[i] = '\0';

	return str;
}

// for itoa()
void swap(char* a, char* b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

// reverse string
// for itoa()
char* revstr(char* str, int start, int end) {
	while(start < end)
		swap(&str[start++], &str[end--]);
	return str;
}

