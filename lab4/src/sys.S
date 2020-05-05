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
call_sys_fork: // (fn, arg, stack)
    /* Save args for the child.  */
    mov	x10, x0					/*fn*/
    mov	x11, x1					/*arg*/
    mov	x12, x2					/*stack*/

    /* Do the system call.  */
    mov x0, x2					/* stack  */
    mov	x8, #SYSNUM_FORK
    svc	0x0

    cmp	x0, #0
    beq	thread_start
    ret

thread_start:
    mov	x29, 0

    /* Pick the function arg and execute.  */
    mov	x0, x11
    blr	x10

    /* We are done, pass the return value through x0.  */
    mov	x8, #SYSNUM_EXIT
    svc	0x0
