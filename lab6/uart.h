#ifndef _UART_H
#define _UART_H

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char* s);
void uart_hex(unsigned int d);
void uart_read_line(char* line);
void uart_print_int(int i);

#endif