void uart_init();
void uart_write(unsigned int c);
char uart_read();
char uart_read_raw();
void uart_printf(char* fmt, ...);
void uart_flush();