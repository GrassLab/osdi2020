#ifndef UART_H
#define UART_H

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
int uart_get_string(char *s);
int uart_get_string_with_echo(char *s);
void uart_puts(char *s);
void uart_hex(unsigned int d);

#endif
