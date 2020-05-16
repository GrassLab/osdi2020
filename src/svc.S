#include "svc.h"
.global sys_get_taskid
sys_get_taskid:
    mov x8, #SYS_GET_TASKID
    svc #0
    ret

.global sys_exec
sys_exec:
    mov x8, #SYS_EXEC
    svc #0
    ret

.global sys_fork
sys_fork:
    mov x8, #SYS_FORK
    svc #0
    ret

.global sys_exit
sys_exit:
    mov x8, #SYS_EXIT
    svc #0
    ret

.global sys_uart_read
sys_uart_read:
    mov x8, #SYS_UART_READ
    svc #0
    ret

.global sys_uart_write
sys_uart_write:
    mov x8, #SYS_UART_WRITE
    svc #0
    ret

.global sys_sched_yield
sys_sched_yield:
    mov x8, #SYS_SCHED_YIELD
    svc #0
    ret