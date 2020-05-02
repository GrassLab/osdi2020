.section ".text"

#include "kernel/exception/sys_call_def.h"

.global default_task_start
.global exec
.global do_exit
.global get_pid
.global schedule
.global fork

default_task_start:
    blr     x19     // an user mode code 

    # after user mode done, do this routine
    bl      do_exit 

exec:
    mov     x8,     SYS_DO_EXEC
    svc     #0
    ret

do_exit:
    mov     x8,     SYS_DO_EXIT
    svc     #0
    ret

get_pid: 
    mov     x8,     SYS_GET_PID
    svc     #0
    ret

schedule:
    mov     x8,     SYS_SCHEDULE
    svc     #0
    ret

fork:
    mov     x8,     SYS_FORK
    svc     #0
    ret