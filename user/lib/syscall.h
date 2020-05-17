#ifndef SYSCALL
#define SYSCALL
#include <stddef.h>
#include "mmap.h"
extern size_t uart_read (char *buf, size_t count);
extern size_t uart_write (char *buf, size_t count);
extern int exec (void (*func) ());
extern size_t get_task_id ();
extern int fork ();
extern void exit (int status);
extern int kill (size_t pid, int signal);
extern void page_status (int *free, int *alloc);
extern void get_time (size_t *cnt, size_t *freq);
extern void *mmap (void *addr, size_t len, int prot,
		   int flags, void *file_start, int file_offset);
#endif /* ifndef SYSCALL */
