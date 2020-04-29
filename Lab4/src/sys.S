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

.global kill
kill:
	mov x8,SYS_KILL
	svc #0
	ret

.global fork
fork:	
	mov x8,SYS_FORK
	svc #0
	ret

.global exec
exec:
	mov x8,SYS_EXEC
	svc #0
	ret

.global exit
exit:
	mov x8,SYS_EXIT
	svc #0
	ret

.global get_taskid
get_taskid:
	mov x8,SYS_GET_TASKID
	svc #0
	ret

.global uart_write
uart_write:
	mov x8,SYS_UART_WRITE
	svc #0
	ret

.global uart_read
uart_read:
	mov x8,SYS_UART_READ
	svc #0
	ret

	
.global get_priority
get_priority:
	mov x8,SYS_GET_PRIORITY
	svc #0
	ret
