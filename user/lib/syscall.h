#ifndef USER_LIB_SYSCALL_H_
#define USER_LIB_SYSCALL_H_

#include "user/lib/types.h"

uint32_t get_taskid(void);
size_t uart_read(void *buf, size_t count);
size_t uart_write(const void *buf, size_t count);
int exec(void(*func)(void));
int fork(void);
void exit(int status);
int kill(uint32_t id, int sig);

#endif // USER_LIB_SYSCALL_H_
