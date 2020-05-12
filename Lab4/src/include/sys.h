#ifndef	_SYS_H
#define	_SYS_H

#define CORE_TIMER    0   // syscal numbers
#define DAIF     1
#define SYS_KILL 2
#define SYS_FORK 3
#define SYS_EXEC 4
#define SYS_EXIT 5
#define SYS_GET_TASKID 6
#define SYS_UART_WRITE 7
#define SYS_UART_READ  8
#define SYS_GET_PRIORITY 9

#ifndef __ASSEMBLER__ 
int call_core_timer();
void call_daif();

//Required system call
int fork();
void kill(int pid,int signal);
int exec(void(* func));
void exit(int status);
int get_taskid();
size_t uart_write(char *buf, size_t size);
size_t uart_read(char *buf,size_t size);
int get_priority();

#endif

#endif  /*_SYS_H */
