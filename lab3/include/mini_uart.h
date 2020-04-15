#ifndef	_MINI_UART_H
#define	_MINI_UART_H

void uart_init ( void );
char uart_recv ( void );
void uart_send ( char c );
void uart_send_string(char* str);
void putc ( void* p, char c );
int readline(char *buf, int maxlen);
void uart_send_int(int number);
void uart_send_hex(unsigned long number);
void handle_uart_irq();
#endif  /*_MINI_UART_H */
