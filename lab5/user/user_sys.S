.section ".text.user"

.set SYS_WRITE_NUMBER       ,0        // syscal numbers
.set SYS_MALLOC_NUMBER      ,1
.set SYS_CLONE_NUMBER       ,2
.set SYS_EXIT_NUMBER        ,3
.set SYS_FORK_NUMBER        ,4
.set SYS_GETID_NUMBER       ,5
.set SYS_TIMESTAMP_NUMBER   ,6
.set SYS_EXEC_NUMBER        ,7
.set SYS_READ_NUMBER        ,8

call_sys_read:
    mov w8, #SYS_READ_NUMBER
    svc #0
    ret

call_sys_exec:
    mov w8, #SYS_EXEC_NUMBER
    svc #0
    ret

call_sys_timestamp:
    mov w8, #SYS_TIMESTAMP_NUMBER
    svc #0
    ret

call_sys_getid:
    mov w8, #SYS_GETID_NUMBER
    svc #0
    ret

call_sys_fork:
    mov w8, #SYS_FORK_NUMBER
    svc #0
    ret

call_sys_write:
    mov w8, #SYS_WRITE_NUMBER
    svc #0
    ret

call_sys_malloc:
    mov w8, #SYS_MALLOC_NUMBER
    svc #0
    ret

call_sys_exit:
    mov w8, #SYS_EXIT_NUMBER
    svc #0
    ret

call_sys_clone: // (fn, arg, stack)
    /* Save args for the child.  */
    mov	x10, x0					/*fn*/
    mov	x11, x1					/*arg*/
    mov	x12, x2					/*stack*/

    /* Do the system call.  */
    mov x0, x2					/* stack  */
    mov	x8, #SYS_CLONE_NUMBER
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
    mov	x8, #SYS_EXIT_NUMBER
    svc	0x0
