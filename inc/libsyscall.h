#ifndef LIBSYSCALL_H
#define LIBSYSCALL_H
#include <stddef.h>
size_t sys_uart_write(const char buf[], size_t size);
size_t sys_uart_read(char buf[], size_t size);
void sys_exec(void (*func)());
int sys_fork();
void sys_exit(int status);
void sys_kill(int pid, int signal);
#endif
