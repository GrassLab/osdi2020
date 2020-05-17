#ifndef USER_LIB_UTILS_H_
#define USER_LIB_UTILS_H_

#include <stdint.h>

int32_t ctoi(char c);
int isspace(char c);
uint64_t pow(uint32_t base, uint32_t exponent);
uint64_t stoui(const char *s, int base);
char *uitos_generic(uint64_t num, int base, char *buf);
char *uitos(uint64_t num, char *buf);

#endif // USER_LIB_UTILS_H_
