#ifndef MYSTD_H
#define MYSTD_H

typedef enum{
	false,
	true
} bool;

int strlen(char *s);
int strcmp(char *s1, char *s2);
void ullToStr(unsigned long long num, char *buf);
void ullToStr_hex(unsigned long long num, char *buf);
void cntTimeStamp(unsigned long long cntfrq, unsigned long long cntpct, char *timeStr);
void printASCII(char c);
void strAppend(char *s1, char *s2);

#endif