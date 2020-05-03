#pragma once

#define __NR_syscalls       7

#define SYS_WRITE_NUMBER        0       // syscal numbers
#define SYS_MALLOC_NUMBER       1
#define SYS_CLONE_NUMBER        2
#define SYS_EXIT_NUMBER         3
#define SYS_FORK_NUMBER         4
#define SYS_GETID_NUMBER        5
#define SYS_TIMESTAMP_NUMBER    6

#ifndef __ASSEMBLER__

void sys_write(char * buf);
int sys_fork();

void call_sys_timestamp();
int call_sys_getid();
int call_sys_fork();
void call_sys_write(char * buf);
int call_sys_clone(unsigned long fn, unsigned long arg, unsigned long stack);
unsigned long call_sys_malloc();
void call_sys_exit();

#endif
