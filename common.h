#ifndef _COMMON_H_
#define _COMMON_H_
#include "uart.h"
#define NULL ((void *)0)

#define size_t unsigned int

extern char __bss_end[];
extern char __end[];

#define assert(expr) \
    if (!(expr))     \
    aFailed(__FILE__, __LINE__)

void aFailed(char *file, int line);

#endif
