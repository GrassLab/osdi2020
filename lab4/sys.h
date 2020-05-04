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

int sys_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss);
int sys_timer_int(void);
int sys_uart_puts(char * string);
int sys_uart_gets(char * string, char delimiter, unsigned length);
int sys_exec(void(*start_func)());
int sys_fork(struct trapframe_struct * trapframe);
int sys_exit(int status);

extern struct task_struct kernel_task_pool[TASK_POOL_SIZE];
extern uint16_t task_kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE];
extern uint16_t task_user_stack_pool[TASK_POOL_SIZE][TASK_USER_STACK_SIZE];

extern void __sys_fork_child_entry(void);
extern int schedule_zombie_exist;

#endif

