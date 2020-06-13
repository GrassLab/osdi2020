#include <stdint.h>
#include <stddef.h>
#ifndef __SYSCALL_H_
#define __SYSCALL_H_

int syscall_uart_puts(char * string);
int syscall_uart_gets(char * string, char delimiter, unsigned length);
int syscall_exec(void(*start_func)());
int syscall_fork(void);
int syscall_exit(int status);
int syscall_signal(int task_id, int signalno);
int syscall_malloc(unsigned bytes);
int syscall_free(uint64_t * va);
int syscall_open(const char * pathname, int flags);
int syscall_close(int fd);
int syscall_write(int fd, const void * buf, size_t len);
int syscall_read(int fd, void * buf, size_t len);
int syscall_list(int fd);
int syscall_mkdir(const char * pathname);
int syscall_chdir(const char * pathname);
int syscall_mount(const char * device, const char * mountpoint, const char * filesystem);

#endif

