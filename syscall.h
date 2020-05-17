#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "uart.h"

#define syscall(x) asm volatile("mov x0, " #x ";svc #0x80");
#define syscall_1(x, a0) asm volatile("mov x1, " #a0 ";mov x0, " #x ";svc #0x80");

void syscall_router(unsigned long, unsigned long, unsigned long, unsigned long);

double gettime();
void core_timer();
int fork();
int exec();
void exit(int);
int get_taskid();
void kill(int task_id, int signal);

void uart_send(unsigned int c);
char uart_recv();
#endif
