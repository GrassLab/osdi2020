#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "types.h"

uint32_t do_get_taskid(void);
size_t do_uart_read(void *buf, size_t count);
size_t do_uart_write(const void *buf, size_t count);
void do_exec(void(*func)(void));
int do_fork(void);
void do_exit(int status);
int do_kill(uint32_t id, int sig);

void el1_to_el0(void(*func)(void), uint8_t *ustack);
void post_fork_child_hook(void);
int do_fork_helper(uint64_t trapframe, uint64_t lr);

#endif // SYSCALL_H_
