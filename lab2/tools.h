#ifndef TOOLS_H
#define TOOLS_H

void reverse(char str[], int length);
char* itoa(unsigned long long int num, char* str, unsigned long long int base);
unsigned long long int atoulli(char* str);
int strcmp(const char *X, const char *Y);
int strlen(const char *str);
int hexChar2int(char ch);
void timestamp(int *integer_part, int *decimal_point_part);
void naive_memcpy(void *destt, void *sourcet, size_t size);

#endif
