#ifndef _USER_LIB_H_
#define _USER_LIB_H_

#define NULL ((void *)0)

char uart_read();
void uart_write(char *c);
int printf(const char *fmt, ...);

#endif