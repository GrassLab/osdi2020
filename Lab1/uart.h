
//specific for our rasp
#ifndef UART
#define UART
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
#endif