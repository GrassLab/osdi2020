void mini_uart_init();
void PL011_uart_init();
void uart_puts(char *s);
void uart_hex(unsigned int d);
char uart_gets(char *s);
void uart_dec(unsigned long i);
void uart_double(double time);

extern void (*uart_send)(unsigned int );
extern char (*uart_getc)();
