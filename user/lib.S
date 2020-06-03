.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl exit

.set SYS_CORE_TIMER_IRQ_ENABLE,   0
.set SYS_GET_TASKID,              1
.set SYS_EXIT,                    2
.set SYS_FORK,                    3    
.set SYS_UART_READ,               4
.set SYS_UART_WRITE,              5   
.set SYS_KILL,                    6
.set SYS_EXEC,                    7

.global call_sys_get_taskid
call_sys_get_taskid:
    mov x8, #SYS_GET_TASKID
    svc #0
    ret

.global exec
exec:
    mov x8, #SYS_EXEC
    svc #0
    ret

.global fork
fork:
    mov x8, #SYS_FORK
    svc #0
    ret

.global exit
exit:
    mov x8, #SYS_EXIT
    svc #0
    ret

.global call_sys_uart_read
call_sys_uart_read:
    mov x8, #SYS_UART_READ
    svc #0
    ret

.global call_sys_uart_write
call_sys_uart_write:
    mov x8, #SYS_UART_WRITE
    svc #0
    ret