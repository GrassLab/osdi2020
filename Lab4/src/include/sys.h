#ifndef	_SYS_H
#define	_SYS_H

#define CORE_TIMER    0   // syscal numbers
#define DAIF     1
#define SYS_MALLOC 2
#define SYS_FORK 3
#define SYS_EXEC 4
#define SYS_EXIT 5
#define SYS_GET_TASKID 6
#define SYS_UART_WRITE 7

#ifndef __ASSEMBLER__
int call_core_timer();
void call_daif();
unsigned long call_sys_malloc();
void call_sys_exit();

//Required system call
int fork();
int exec(void(* func));
void exit(int status);
int get_taskid();
size_t uart_write(const char buf[], size_t size);
#endif

#endif  /*_SYS_H */
