#ifndef UART0_BUF
#define UART0_BUF

#include "queue.h"

#define UART0_BUF_MAX_SIZE 1024

struct queue read_buf, write_buf;

#endif

void uart_init();
void uart_write(char c);
char uart_read();
void uart_printf(char* fmt, ...);
void uart_flush();