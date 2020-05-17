#ifndef __UART0_H__
#define __UART0_H__

#include "queue.h"
#define UART0_BUF_MAX_SIZE 1024
extern struct uart_queue read_buf, write_buf;

#endif

void uart_init();
void uart0_write(char c);
char uart0_read();
void uart_printf(char* fmt, ...);
void uart_flush();