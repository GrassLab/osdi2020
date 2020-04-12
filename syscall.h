#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "uart.h"

void svc(int num);

void brk(int num);

/* FAIL dont use */
void syscall(unsigned int, unsigned int, unsigned int, unsigned int);
#endif