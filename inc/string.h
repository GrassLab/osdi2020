#ifndef _STRING_H
#define _STRING_H 

int strcmp(const char* s1, const char* s2);
void unitoa(unsigned num, char *buff, unsigned num_dig);
int atoi(char *ascii);
void bin2hex(unsigned int num, char buff[11]);
int hex2int(char *hex);

#endif//_STRING_H