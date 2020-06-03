#include <stdint.h>
#ifndef __SYSCALL_H_
#define __SYSCALL_H_

int syscall_uart_puts(char * string);
int syscall_uart_gets(char * string, char delimiter, unsigned length);
int syscall_exec(void(*start_func)());
int syscall_fork(void);
int syscall_exit(int status);
int syscall_signal(int task_id, int signalno);
int syscall_malloc(unsigned bytes);
int syscall_free(uint64_t * va);

#endif

