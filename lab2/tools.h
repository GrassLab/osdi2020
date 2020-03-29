#ifndef TOOLS_H
#define TOOLS_H

void reverse(char str[], int length);
char* itoa(unsigned long long int num, char* str, unsigned long long int base);
int strcmp(const char *X, const char *Y);
int strlen(const char *str);
int hexChar2int(char ch);
void timestamp(int *integer_part, int *decimal_point_part);

#endif
