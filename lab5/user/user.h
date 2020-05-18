#ifndef	_USER_H
#define	_USER_H

void user_process();

extern unsigned long user_begin;
extern unsigned long user_end;
extern void call_sys_write(char *buf);

#endif
