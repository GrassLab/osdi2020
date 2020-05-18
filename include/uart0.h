#ifndef __UART0_H__
#define __UART0_H__

#define UART_QUEUE_MAX_SIZE 2048

struct uart_queue {  // circular queue
    int front;
    int rear;
    int max;
    char buf[UART_QUEUE_MAX_SIZE];
};

#define UART0_BUF_MAX_SIZE 1024

#endif

extern struct uart_queue read_buf, write_buf;

void uart_queue_init(struct uart_queue* q, int max);
int uart_queue_empty(struct uart_queue* q);
int uart_queue_full(struct uart_queue* q);
void uart_queue_push(struct uart_queue* q, char val);
char uart_queue_pop(struct uart_queue* q);

void uart0_init();
void uart0_write(char c);
char uart0_read();
void uart_printf(char* fmt, ...);
void uart0_flush();