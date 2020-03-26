#ifndef	_UART_H
#define	_UART_H

void uart_init();
void uart_send_string(char *str);
void uart_send(char c);
char uart_recv();
void uart_send_int(int number);
int uart_read_int();
void uart_send_hex(unsigned long number);
int readline(char *buf, int maxlen);
long long int read_kernel_address();
#endif  /*_UART_H */