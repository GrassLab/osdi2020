.section ".text"

#include "kernel/exception/sys_call_def.h"

.global get_time_stamp
.global wait_msec

get_time_stamp:
    mov     x8,     SYS_GET_TIMESTAMP
    svc     #0
    ret

wait_msec:
    mov     x8,     SYS_WAIT_MSEC
    svc     #0
    ret