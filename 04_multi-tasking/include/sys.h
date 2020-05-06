#ifndef _SYS_H
#define _SYS_H

#define __NR_syscalls   6

#define SYS_UART_READ   0
#define SYS_UART_WRITE  1
#define SYS_EXEC        2
#define SYS_FORK        3
#define SYS_EXIT        4

#ifndef __ASSEMBLER__

typedef unsigned long size_t;

size_t uart_read(char*, size_t);
size_t uart_write(const char*, size_t);
int exec(unsigned long);
int fork();
void exit(int);

int handle_el0_sync(unsigned long, unsigned long);
// void handle_el1_sync(unsigned long, unsigned long);

#endif

#endif
