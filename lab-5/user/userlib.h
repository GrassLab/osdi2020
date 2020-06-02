#ifndef USERLIB_H
#define USERLIB_H

#define SYSCALL_GET_TASK_ID 0
#define SYSCALL_UART_READ 1
#define SYSCALL_UART_WRITE 2
#define SYSCALL_EXEC 3
#define SYSCALL_GET_TIMESTAMP 4
#define SYSCALL_ENABLE_IRQ 5
#define SYSCALL_DISABLE_IRQ 6
#define SYSCALL_CORE_TIMER_INIT 7
#define SYSCALL_LOCAL_TIMER_INIT 8
#define SYSCALL_SYS_TIMER_INIT 9
#define SYSCALL_EXIT 10
#define SYSCALL_FORK 11

int get_taskid();
int __get_taskid();
void get_timestamp();
void __get_timestamp();
int fork();
int exec(void(*func)());
void exit(int status);

#endif