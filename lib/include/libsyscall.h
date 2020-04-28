#ifndef LIBSYSCALL_H_
#define LIBSYSCALL_H_

#include "types.h"

uint32_t get_taskid(void);
size_t uart_read(void *buf, size_t count);
size_t uart_write(const void *buf, size_t count);

#endif // LIBSYSCALL_H_
