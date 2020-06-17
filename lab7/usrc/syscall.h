#ifndef __SYSCALL_H__
#define __SYSCALL_H__

//#define __NR_syscalls  6
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
#define SYSNUM_MMAP 10
#define SYSNUM_PAGES 11
#define SYSNUM_FOPEN 12
#define SYSNUM_FCLOSE 13
#define SYSNUM_FREAD 14
#define SYSNUM_FWRITE 15

#define PROT_NONE  0b0001
#define PROT_READ  0b0010
#define PROT_WRITE 0b0100
#define PROT_EXEC  0b1000

#define MAP_FIXED     0b001
#define MAP_ANONYMOUS 0b010
#define MAP_POPULATE  0b100

#define MAP_FAILED ((void*)(-1))

#ifndef __ASSEMBLER__

#include "type.h"
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
void *call_sys_mmap(void* addr, unsigned long len,
    int prot, int flags, int file_start, int file_offset);
unsigned long call_sys_pages(unsigned long pid);
int call_sys_fopen(char *path, int flags);
int call_sys_fclose(int fd);
int call_sys_fread(int fd, char *buf, size_t len);
int call_sys_fwrite(int fd, char *buf, size_t len);

#endif

#endif
