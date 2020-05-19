#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define __NR_syscalls  6
#define SYSNUM_READ    0
#define SYSNUM_WRITE   1
#define SYSNUM_EXEC    2
#define SYSNUM_FORK    3
#define SYSNUM_EXIT    4
#define SYSNUM_SIGNAL  5
#define SYSNUM_MUTEX_LOCK    6
#define SYSNUM_MUTEX_UNLOCK    7
#define SYSNUM_TASK_ID 8
#define SYSNUM_PAGE_NUM 9

#ifndef __ASSEMBLER__

#include "mutex.h"

int call_sys_read();
void call_sys_write(char *);
int call_sys_exec();
int call_sys_fork();
void call_sys_exit(int status);
void call_sys_signal(unsigned long, int);
void call_sys_mutex_lock(Mutex *);
void call_sys_mutex_unlock(Mutex *);
unsigned long call_sys_task_id();
unsigned long call_sys_page_num();
#endif

#endif
