#define __ASSEMBLY__
#include "sys.h"

.global get_taskid
get_taskid:
    mov x8, SYS_GET_TASK_ID
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

.global remain_page_num
remain_page_num:
    mov x8, SYS_REMAIN_PAGE
    svc #0
    ret

.global obj_alloc_init
obj_alloc_init:
    mov x8, SYS_OBJ_ALLOC_INIT
    svc #0
    ret
