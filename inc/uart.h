void uart_init();
void uart_send(unsigned int c);
char uart_getb();
char uart_getc();
void uart_puts(const char *s);
void uart_hex(unsigned int d);
void uart_flush();
int print(char *fmt, ...);
