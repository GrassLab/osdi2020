#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "type.h"

int32_t fork();
int32_t exec(void (*func)());
void exit(int32_t status);
uint32_t getTaskId();
void printf(char* c);
char uartRead();
void userTest();

#endif