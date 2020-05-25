#ifndef SYSCALL_H
#define SYSCALL_H
#include <stddef.h>
size_t sys_uart_write(const char buf[], size_t size);
size_t sys_uart_read(char buf[], size_t size);
void sys_exec(void (*func)());
int sys_fork();
void sys_exit(int status);
void sys_kill(int pid, int signal);
char sys_uart_getc();
#endif

