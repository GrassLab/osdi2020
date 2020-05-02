#ifndef SYSCALL
#define SYSCALL
#include <stddef.h>
extern size_t uart_read (char *buf, size_t count);
extern size_t uart_write (char *buf, size_t count);
extern int exec (void (*func) ());
#endif /* ifndef SYSCALL */
