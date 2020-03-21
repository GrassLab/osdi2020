#ifndef	_UART_H
#define	_UART_H

void uart_init();
void uart_send_string(char *str);
void uart_send(char c);
char uart_recv();
void uart_send_int(int number);
int uart_read_int();

#endif  /*_UART_H */