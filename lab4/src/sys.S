#include "sys.h"

.globl call_sys_write
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER	
	svc #0
	ret

.globl call_sys_read
call_sys_read:
	mov w8, #SYS_READ_NUMBER	
	svc #0
	ret

.globl call_sys_malloc
call_sys_malloc:
	mov w8, #SYS_MALLOC_NUMBER	
	svc #0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER	
	svc #0
	ret

.globl  call_sys_chk_exl
call_sys_chk_exl:
    mov w8, #SYS_CHK_EXL	
	svc #0
	ret

.globl  call_sys_chk_exl_info
call_sys_chk_exl_info:
    mov w8, #SYS_CHK_EXL_INFO	
	svc #0
	ret

.globl  call_sys_enable_time
call_sys_enable_time:
    mov w8, #SYS_ENABLE_TIME	
	svc #0
	ret

.globl  call_sys_get_taskid
call_sys_get_taskid:
    mov w8, #SYS_GET_TASKID	
	svc #0
	ret

.globl call_sys_clone
call_sys_clone:
	/* Save args for the child.  */
	mov	x10, x0					/*fn*/
	mov	x11, x1					/*arg*/
	mov	x12, x2					/*stack*/

	/* Do the system call.  */
	mov 	x0, x2					/* stack  */
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