.section ".text.entry"

#define SYS_PRINT 0
#define SYS_SCAN 1
#define SYS_EXEC 2
#define SYS_FORK 3
#define SYS_EXIT 4
#define SYS_KILL 5
#define SYS_READ 7

.global _user_entry
_user_entry:
	bl main
	bl sys_exit

.global sys_uart_write
sys_uart_write:
    mov x8, #SYS_PRINT
    svc 1
    ret

.global sys_uart_read
sys_uart_read:
    mov x8, #SYS_SCAN
    svc 1
    ret

.global sys_fork
sys_fork:
    mov x8, #SYS_FORK
    svc 1
    ret

.global sys_exit
sys_exit:
    mov x8, #SYS_EXIT
    svc 1
    ret

.global sys_uart_getc
sys_uart_getc:
    mov x8, #SYS_READ
    svc 1
    ret
