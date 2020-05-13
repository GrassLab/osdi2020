#ifndef __SYSCALL__
#define __SYSCALL__


#define SYS_GET_TASKID 15
#define SYS_UART_READ 16
#define SYS_UART_WRITE 17
#define SYS_EXEC 19
#define SYS_FORK 20

int get_taskid();
void get_input();
void printf_(char *c);
#endif