#include "include/sys.h"

.global call_core_timer
call_core_timer:
	mov x8,CORE_TIMER //x8 is for syscall number
        svc #0
        ret

.global call_sys_timer
call_sys_timer:
	mov x8,SYS_TIMER
        svc #0
        ret
