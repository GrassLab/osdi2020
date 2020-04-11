#define MMIO_BASE 0x3F000000
char uart_img();
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_hex_64(unsigned long long int d);
void wait_cycles(unsigned int n);
void printf(char *fmt, ...);