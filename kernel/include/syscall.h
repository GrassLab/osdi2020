#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "types.h"

uint32_t do_get_taskid(void);

void *syscall_table[] = {
  do_get_taskid
};

#endif // SYSCALL_H_
