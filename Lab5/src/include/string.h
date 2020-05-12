#ifndef _STRING_H
#define _STRING_H

typedef unsigned long size_t;

int strcmp(const char* p1, const char* p2);
int itos(unsigned long long num,char* str,int base);
int get_length(char str[]);
void reverse_string(char *str);

#endif  /*_STRING_H */

