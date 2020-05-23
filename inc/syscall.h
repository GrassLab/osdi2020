#ifndef SYSCALL_H
#define SYSCALL_H
#include <stdint.h>
#include <trap.h>

#define SYS_PRINT 0
#define SYS_SCAN 1
#define SYS_EXEC 2
#define SYS_FORK 3
#define SYS_EXIT 4
#define SYS_KILL 5
#define SYS_READ 7

void do_exec(uint8_t* func, int size);
void do_fork(uint64_t elr);
void do_exit(uint64_t status);
void kexit(uint64_t status);
void do_kill(uint64_t pid, uint64_t signal);
void syscall_handler(int syscall, struct trap_frame_t* trap_frame,
                     uint64_t elr);
#endif
