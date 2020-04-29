#include "sys.h"
.globl sync_call_time
sync_call_time:
    mov w8, #1
    svc #0
    ret

.globl sync_call_clone
sync_call_clone:
    /* save register for child */
    mov x10, x0 /*fn */
    mov x11, x1 /*arg */
    mov w8, #2
    svc #0
    
    cmp	x0, #0
    beq	thread_start
    ret
thread_start:
    mov	x29, 0 //???
    /* Pick the function arg and execute.  */
    mov x0, x11
    blr x10
    mov	w8, #SYS_EXIT_NUMBER
    svc	0x0

