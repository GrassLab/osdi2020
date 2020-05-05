#ifndef SYSCALL_H
#define SYSCALL_H
void uart_write(char ch);
char uart_read();
void exec(void (*func)());
int fork();
#endif
