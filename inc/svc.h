#ifndef _SVC_H
#define _SVC_H

#define __NR_syscalls	            7

#define SYS_UART_READ               0
#define SYS_UART_WRITE              1
#define SYS_GET_TASKID              2
#define SYS_EXEC                    3
#define SYS_FORK                    4
#define SYS_EXIT                    5
#define SYS_KILL                    6
#define SYS_SCHED_YIELD             7

#ifndef __ASSEMBLER__
unsigned long sys_get_taskid();
unsigned long sys_exec(void(*func)());
int sys_fork();
unsigned long sys_exit();
char sys_uart_read();
void sys_uart_write(char *s);
void sys_sched_yield();
#endif//__ASSEMBLER__


#endif//_SVC_H