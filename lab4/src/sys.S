#include "sys.h"

.globl call_sys_read
call_sys_read:
    mov w8, #SYSNUM_READ
    svc #0
    ret

.globl call_sys_write
call_sys_write:
    mov w8, #SYSNUM_WRITE
    svc #0
    ret

.globl call_sys_exec
call_sys_exec:
    mov w8, #SYSNUM_EXEC
    svc #0
    ret

.globl call_sys_exit
call_sys_exit:
    mov w8, #SYSNUM_EXIT
    svc #0
    ret

.globl call_sys_fork
call_sys_fork:
    mov w8, #SYSNUM_FORK
    svc #0
    ret

.globl call_sys_signal
call_sys_signal:
    mov w8, #SYSNUM_SIGNAL
    svc #0
    ret
