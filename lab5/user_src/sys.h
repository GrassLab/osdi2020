#ifndef __SYS_H__
#define __SYS_H__

#define __NR_syscalls  6
#define SYSNUM_READ    0
#define SYSNUM_WRITE   1
#define SYSNUM_EXEC    2
#define SYSNUM_FORK    3
#define SYSNUM_EXIT    4
#define SYSNUM_SIGNAL  5
#define SYSNUM_MUTEX_LOCK    6
#define SYSNUM_MUTEX_UNLOCK    7


#define support_mutex 0

#ifndef __ASSEMBLER__
int call_sys_read();
void call_sys_write(char *);
int call_sys_exec();
int call_sys_fork();
void call_sys_exit();
void call_sys_signal(unsigned long, int);

#if support_mutex
#include "task.h"
void call_sys_mutex_lock(Mutex *);
void call_sys_mutex_unlock(Mutex *);
#endif

#endif

#endif
