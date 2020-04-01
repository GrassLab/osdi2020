#ifndef _UART_INCLUDE_H_
#define _UART_INCLUDE_H_

void uart_init();
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned long long int d);
#endif