#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "uart.h"

#define syscall(x) asm volatile("mov x0, " #x ";svc #0x80");
#define syscall1(x, a0) asm volatile("mov x1, "  #a0 ";mov x0, " #x ";svc #0x80");

/* FAIL dont use */
void syscall_router(unsigned long, unsigned long, unsigned long, unsigned long);
#endif
