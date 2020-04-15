#include "uart.h"

void printASCII(char c){
	int asc = c;
	uart_send('\n');
	uart_send('0'+asc/100);
	asc = asc%100;
	uart_send('0'+asc/10);
	asc = asc % 10;
	uart_send('0'+asc);
	uart_send('\n');
}

int strlen(char *s){
	int size = 0;

	while(*s) {
		size++;
		s++;
	}

	return size;
}

int strcmp(char *s1, char *s2){
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	if(len1 != len2) return 0;
	for(int i=0; i<=len1; i++){
		if(*s1 != *s2) return 0;
		s1++;
		s2++;
	}

	return 1;
}

void strReverse(char *buf, int size){
	int front = 0;
	int last = size;

	while(last > front){
		char temp;
		temp = buf[front];
		buf[front] = buf[last];
		buf[last] = temp;

		front++;
		last--;
	}
}

void ullToStr(unsigned long long num, char *buf){
	if(num==0){
		buf[0] = '0';
		buf[1] = '\0';

		return;
	}

	int size=0;
	while(num){
		buf[size] = '0' + num%10;

		size++;
		num = num / 10;
	}

	buf[size] = '\0';
	strReverse(buf, size-1);

	return;
}

void cntTimeStamp(unsigned long long cntfrq, unsigned long long cntpct, char *timeStr){
	unsigned long long quote = cntpct / cntfrq;

	unsigned remainder = cntpct - cntfrq*quote;

	int iteration = 10;
	int decimalPoint = 0;
	while(remainder && iteration){
		remainder = remainder * 10;
		int q = remainder / cntfrq;
		decimalPoint = decimalPoint*10 + q;
		remainder = remainder - cntfrq*q;

		iteration--;
	}

	char quoteStr[65];
	char decimalPointStr[65];

	ullToStr(quote, quoteStr);
	ullToStr(decimalPoint, decimalPointStr);

	int quoteLen = strlen(quoteStr);
	int decimalPointLen = strlen(decimalPointStr);

	int pos=0;
	for(int i=0; i<quoteLen; i++){
		timeStr[pos] = quoteStr[i];
		pos++;
	}
	timeStr[pos] = '.';
	pos++;
	for(int i=0; i<decimalPointLen; i++){
		timeStr[pos] = decimalPointStr[i];
		pos++;	
	}
	timeStr[pos] = '\0';
	pos++;
}