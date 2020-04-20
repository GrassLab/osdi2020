#include "include/sys.h"

.global call_core_timer
call_core_timer:
	mov x8,CORE_TIMER //x8 is for syscall number
        svc #0
        ret

.global call_daif
call_daif:
	mov x8,DAIF
	svc #0
	ret

.global call_sys_malloc
call_sys_malloc:
	mov x8,SYS_MALLOC
	svc #0
	ret

.global call_sys_clone
call_sys_clone:
	mov x10,x0 //fn
	mov x11,x1 //arg
	mov x12,x2 //stack
	
	mov x0,x2
	mov x8,SYS_CLONE
	svc #0
	// After exception: if return 0, we are executing inside of the newly created thread. 
	cmp x0,#0	
	beq thread_start
	// Return here right after the syscall finishes and we are executing inside the original thread
	ret

thread_start:
	mov x29,0
	// excute function with args
	mov x0,x11
	blr x10
	
	mov x8,#SYS_EXIT
	svc #0

.global call_sys_exit
call_sys_exit:
	mov x8,SYS_EXIT
	svc #0
	ret

