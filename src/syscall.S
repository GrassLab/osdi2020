#include "../include/syscall.h"

.global call_sys_get_taskid
call_sys_get_taskid:
    mov x8, #SYS_GET_TASKID
    svc #0
    ret

.global call_sys_exec
call_sys_exec:
    mov x8, #SYS_EXEC
    svc #0
    ret

.global call_sys_fork
call_sys_fork:
    mov x8, #SYS_FORK
    svc #0
    ret

.global call_sys_exit
call_sys_exit:
    mov x8, #SYS_EXIT
    svc #0
    ret

.global call_sys_uart_read
call_sys_uart_read:
    mov x8, #SYS_UART_READ
    svc #0
    ret

.global call_sys_uart_write
call_sys_uart_write:
    mov x8, #SYS_UART_WRITE
    svc #0
    ret