#ifndef MYSTD_H
#define MYSTD_H

int strlen(char *s);
int strcmp(char *s1, char *s2);
void ullToStr(unsigned long long num, char *buf);
void cntTimeStamp(unsigned long long cntfrq, unsigned long long cntpct, char *timeStr);
void printASCII(char c);

#endif