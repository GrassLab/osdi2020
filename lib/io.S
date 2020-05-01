.section ".text"

#include "kernel/exception/sys_call_def.h"

.global uart_write
.global uart_read

uart_write:
    mov     x8,     SYS_UART_WRITE
    svc     #0
    ret

uart_read:
    mov     x8,     SYS_UART_READ
    svc     #0
    ret