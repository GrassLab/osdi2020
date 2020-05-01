#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "type.h"

int32_t fork();
int32_t exec(void (*func)());
void exit(int32_t status);

#endif