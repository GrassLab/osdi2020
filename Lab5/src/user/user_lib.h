#ifndef	_USER_LIB_H
#define	_USER_LIB_H

int call_core_timer();
void call_daif();
int fork();
void kill(int pid,int signal);
int exec(void(* func));
void exit(int status);
int get_taskid();
unsigned long uart_write(char *buf, unsigned long size);
unsigned long uart_read(char *buf,unsigned long size);
int user_printf(char *fmt, ...);

#endif  
