#ifndef _UART_H_
#define _UART_H_

void uart_init();
char uart_getc();
void uart_puts(char *s);

void uart_send_int(int);
void uart_send_hex(unsigned int);

int uart_gets(char *buf, int buf_size);

void putc(void *p, char c);
#endif
