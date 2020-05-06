#define IN_ASM
#include "sys.h"

.global get_cntfrq
get_cntfrq:
    mov x8, SYS_GET_CNTFRQ
    svc #0
    ret

.global get_cntpct
get_cntpct:
    mov x8, SYS_GET_CNTPCT
    svc #0
    ret

.global uart_read
uart_read:
    mov x8, SYS_UART_READ
    svc #0
    ret

.global uart_write
uart_write:
    mov x8, SYS_UART_WRITE
    svc #0
    ret

.global exec
exec:
    mov x8, SYS_EXEC
    svc #0
    ret

.global fork
fork:
    mov x8, SYS_FORK
    svc #0
    ret

.global exit
exit:
    mov x8, SYS_EXIT
    svc #0
    ret

.global kill
kill:
    mov x8, SYS_KILL
    svc #0
    ret
