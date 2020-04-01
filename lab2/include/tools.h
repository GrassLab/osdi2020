#ifndef TOOLS_H
#define TOOLS_H

// for reboot
#define PM_PASSWORD (0x5a000000)
#define PM_RSTC ((int*)0x3F10001c)
#define PM_WDOG ((int*)0x3F100024)

void reverse(char str[], int length);
char* itoa(unsigned long long int num, char* str, unsigned long long int base);
unsigned long long int atoulli(char* str);
int strcmp(const char *X, const char *Y);
int strlen(const char *str);
unsigned long long int hexChar2int(char ch);
void timestamp(int *integer_part, int *decimal_point_part);
void memcpy(void *destt, void *sourcet, int size);

#endif
