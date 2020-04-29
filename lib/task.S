.section ".text"

.global default_task_start
.global do_exec
.global do_exit
.global get_pid

.macro SYS_CALL_DO_EXEC
    svc     #9
.endm

.macro SYS_CALL_DO_EXIT
    svc     #11
.endm

.macro SYS_CALL_GET_PID
    svc     #12
.endm

default_task_start:
    blr     x19     // an user mode code 

    # after user mode done, do this routine
    bl      do_exit 

do_exec:
    SYS_CALL_DO_EXEC
    ret

do_exit:
    SYS_CALL_DO_EXIT
    ret

get_pid: 
    SYS_CALL_GET_PID
    ret

