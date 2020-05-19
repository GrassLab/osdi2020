#ifndef KERNEL_SYSCALL_H_
#define KERNEL_SYSCALL_H_

#include "kernel/exception.h"
#include "kernel/lib/types.h"

uint32_t do_get_taskid(void);
size_t do_uart_read(void *buf, size_t count);
size_t do_uart_write(const void *buf, size_t count);
void do_exec(uint64_t binary_start, size_t binary_size);
int do_fork(void);
void do_exit(int status);
int do_kill(uint32_t id, int sig);

void el1_to_el0(uint64_t entry_point, uint8_t *ustack);
void post_fork_child_hook(void);
int do_fork_helper(struct trapframe *tf, uint64_t lr);

#endif // KERNEL_SYSCALL_H_
