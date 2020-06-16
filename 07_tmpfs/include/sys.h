#ifndef _SYS_H
#define _SYS_H

#define __NR_syscalls               9

#define SYS_UART_READ               0
#define SYS_UART_WRITE              1
#define SYS_EXEC                    2
#define SYS_FORK                    3
#define SYS_EXIT                    4
#define SYS_ALLOCATOR_REGISTER      5
#define SYS_ALLOCATOR_ALLOC         6
#define SYS_ALLOCATOR_FREE          7
#define SYS_ALLOCATOR_UNREGISTER    8
#ifndef __ASSEMBLER__

typedef unsigned long long size_t;

size_t uart_read(char*, size_t);
size_t uart_write(const char*, size_t);
int exec(unsigned long);
int fork();
void exit(int);

int sys_allocator_register(unsigned long);
unsigned long sys_allocator_alloc(int);
void sys_allocator_free(int, unsigned long);
void sys_allocator_unregister(int);

int handle_el0_sync(unsigned long, unsigned long);
// void handle_el1_sync(unsigned long, unsigned long);

#endif

#endif
