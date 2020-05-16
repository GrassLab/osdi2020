#ifndef SYSCALL
#define SYSCALL
#include <stddef.h>
extern size_t uart_read (char *buf, size_t count);
extern size_t uart_write (char *buf, size_t count);
extern int exec (void (*func) ());
extern double get_time ();
extern size_t get_task_id ();
extern int fork ();
extern void exit (int status);
extern int kill (size_t pid, int signal);
extern void page_status (int *free, int *alloc);
#endif /* ifndef SYSCALL */
