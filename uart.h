#ifndef _UART_H_
#define _UART_H_
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);

void uart_send_int(int);
void uart_send_hex(int);
void uart_send_float(float, int);

int print(char *s);
int gets(char *buf, int buf_size);
#endif
