#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
size_t uart_write(const char buf[], size_t size);
size_t uart_read(char buf[], size_t size);
void exec(void (*func)());
int fork();
void exit(int status);

#endif
