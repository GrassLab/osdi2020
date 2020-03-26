#ifndef _UART_H
#define _UART_H

void uart_init();
void uart_send(char c);
char uart_getc();
void uart_hex(unsigned int d);
void uart_send_int(int number);
#endif /*_UART_H */
