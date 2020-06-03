void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
char uart_i2c(unsigned int d);
int uart_atoi(char *dst, int d);
void uart_hex(unsigned int d);
int uart_strncmp(const char *cs, const char *ct, int len);
void uart_memcpy (const void *src, void *dst, int len);
void uart_memset (void *dst, char s, int len);
char* uart_readline();
void printf(char *fmt, ...);