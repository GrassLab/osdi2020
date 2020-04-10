#include "include/sys.h"

.global call_sys_timer
call_sys_timer:
	mov x8,SYS_TIMER //x8 is for syscall number
        svc #0
        ret
