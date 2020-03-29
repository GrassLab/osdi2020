void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
char uart_i2c(unsigned int d);
void uart_hex(unsigned int d);
int uart_strcmp(const char *cs, const char *ct);