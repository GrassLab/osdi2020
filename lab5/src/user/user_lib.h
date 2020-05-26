#ifndef   _USER_LIB_H
#define _USER_LIB_H

int fork();
int exec(void(* func));
void exit(int status);
int get_taskid();
unsigned long uart_write(char *buf, unsigned long size);
unsigned long uart_read(char *buf,unsigned long size);

#endif
