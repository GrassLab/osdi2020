#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "uart.h"

void svc(int num);

void brk(int num);

/* FAIL dont use */
void syscall_router(unsigned long, unsigned long, unsigned long, unsigned long);
#endif