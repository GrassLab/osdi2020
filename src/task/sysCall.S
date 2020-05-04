#include "task/sysCallTable.h"

.globl fork
fork:
    mov x8, FORK
    svc #0
    ret

.globl exec
exec:
    mov x8, EXEC
    svc #0
    ret

.globl exit
exit:
    mov x8, EXIT
    svc #0
    ret

.globl getTaskId
getTaskId:
    mov x8, GET_TASK_ID
    svc #0
    ret

.globl uartWrite
uartWrite:
    mov x8, UART_WRITE
    svc #0
    ret 

.globl uartRead
uartRead:
    mov x8, UART_READ
    svc #0
    ret 