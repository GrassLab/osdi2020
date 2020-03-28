#ifndef	_MINI_UART_H
#define	_MINI_UART_H
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);

#endif  /*_MINI_UART_H*/