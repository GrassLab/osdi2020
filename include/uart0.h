#ifndef __UART0_H__
#define __UART0_H__

#define UART0_BUF_MAX_SIZE 1024

#endif

extern struct queue read_buf, write_buf;

void uart_init();
void uart_write(char c);
char uart_read();
void uart_printf(char* fmt, ...);
void uart_flush();