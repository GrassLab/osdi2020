#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H

#define SYS_RECV_UART 0u
#define SYS_SEND_UART 1u
#define SYS_EXEC 2u
#define SYS_FORK 3u
#define SYS_EXIT 4u

#include <stdint.h>

void handleSVC(uint64_t *trapframe);

#endif
