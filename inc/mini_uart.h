#ifndef	_MINI_UART_H
#define	_MINI_UART_H
void uart_init(void);
void uart_send(char c);
char uart_recv(void);
char uart_getc(void);
void uart_putc(char c);
void uart_puts(char *s);

#endif// _MINI_UART_H