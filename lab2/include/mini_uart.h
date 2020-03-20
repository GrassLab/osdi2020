#ifndef	_MINI_UART_H
#define	_MINI_UART_H

void mini_uart_init(unsigned int baudrate);
void mini_uart_send_string(char *str);
void mini_uart_send(char c);
char mini_uart_recv();

#endif  /*_MINI_UART_H */