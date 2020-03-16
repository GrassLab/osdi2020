void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_print(char *s);
void uart_print_int(int num);
void uart_print_double(double num);