void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void printf(char *fmt, ...);
int strcpy(const char *ct, const char *cs);
int strcmp(const char *ct, const char *cs);
int strncmp(const char *ct, const char *cs, int len);
