#include "gpio.h"
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
int uart_read_int();
void uart_send_int(int number);
int uart_gets(char *buf, int buf_size);
void uart_hex(unsigned int d);