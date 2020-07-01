#ifndef SYS_CALL
#define SYS_CALL

extern char uart_read();

extern void uart_write(char *s);

extern void exec(void(*func)());

extern int fork();



#endif
