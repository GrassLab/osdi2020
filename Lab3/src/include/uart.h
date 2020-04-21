#ifndef _UART_H
#define _UART_H

void uart_init();
void uart_send(char c);
char uart_recv();
void uart_send_string(char *str);
void uart_recv_string(int size,char* buf);
void uart_hex(unsigned int d);
void uart_IRQhandler();

#define BUF_SIZE 0xFFF

int read_buf_head;
int read_buf_tail;
char read_buf[BUF_SIZE];

int write_buf_head;
int write_buf_tail;
char write_buf[BUF_SIZE];


#endif /*_UART_H */
