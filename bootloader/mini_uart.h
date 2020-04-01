#ifndef _MINI_UART_H_
#define _MINI_UART_H_
void mini_uart_init();
void mini_uart_send(unsigned int c);
char mini_uart_getc();
void mini_uart_puts(char *s);

void mini_uart_send_int(int);
void mini_uart_send_hex(int);
void mini_uart_send_float(float, int);

int mini_uart_print(char *s);
int mini_uart_gets(char *buf, int buf_size);
#endif
