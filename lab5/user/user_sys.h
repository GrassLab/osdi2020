#ifndef	_USER_SYS_H
#define	_USER_SYS_H

int call_sys_exec();
void call_sys_timestamp();
int call_sys_getid();
int call_sys_fork();
char call_sys_read();
void call_sys_write(char * buf);
int call_sys_clone(unsigned long fn, unsigned long arg, unsigned long stack);
unsigned long call_sys_malloc();
void call_sys_exit();

#endif  /*_USER_SYS_H */
