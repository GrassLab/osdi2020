#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H

#define SYS_RECV_UART 0u
#define SYS_SEND_UART 1u
#define SYS_EXEC 2u

void handleSVC(void);

#endif
