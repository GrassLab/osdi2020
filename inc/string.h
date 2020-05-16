#ifndef _STRING_H
#define _STRING_H 

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned int n);
void unitoa(unsigned num, char *buff, unsigned num_dig);
int atoi(char *ascii);
int hex2int(char *hex);

#endif//_STRING_H