#include <stdint.h>
#include "task.h"
#include "exc.h"
#ifndef __SYS_H__
#define __SYS_H__

#define SYS_UART_PUTS 3
#define SYS_UART_GETS 4
#define SYS_EXEC 5
#define SYS_FORK 6
#define SYS_EXIT 7
#define SYS_SIGNAL 8
#define SYS_MALLOC 9
#define SYS_FREE 10
#define SYS_OPEN 11
#define SYS_CLOSE 12
#define SYS_WRITE 13
#define SYS_READ 14
#define SYS_LIST 15
#define SYS_MKDIR 16
#define SYS_CHDIR 17
#define SYS_MOUNT 18

int sys_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss);
int sys_timer_int(void);
int sys_uart_puts(char * string);
int sys_uart_gets(char * string, char delimiter, unsigned length);
int sys_exec_todo(void(*start_func)());
int sys_fork(struct trapframe_struct * trapframe);
int sys_exit(int status);
int sys_signal(int task_id, int signalno);
int sys_malloc(unsigned bytes);
int sys_free(uint64_t * va);
int sys_open(const char * pathname, int flags);
int sys_close(int fd);
int sys_write(int fd, const void * buf, size_t len);
int sys_read(int fd, void * buf, size_t len);
int sys_list(int fd);
int sys_mkdir(const char * pathname);
int sys_chdir(const char * pathname);
int sys_mount(const char * device, const char * mountpoint, const char * filesystem);

extern struct task_struct * kernel_task_pool;
extern uint16_t * task_kernel_stack_pool;

extern void __sys_fork_child_entry(void);
extern int schedule_zombie_count;
extern unsigned mmu_page_used;

#endif

