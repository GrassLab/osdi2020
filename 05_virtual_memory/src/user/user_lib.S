.section ".text.entry"

.global _user_entry
_user_entry:
    bl main
    bl exit 

.set SYS_UART_READ, 0
.set SYS_UART_WRITE, 1
.set SYS_EXEC, 2
.set SYS_FORK, 3
.set SYS_EXIT, 4

.global uart_read
uart_read:
    mov x8, #SYS_UART_READ
    svc #0
    ret

.global uart_write
uart_write:
    mov x8, #SYS_UART_WRITE
    svc #0
    ret

.global exec
exec:
    mov w8, #SYS_EXEC
    svc #0
    ret

.global fork
fork:
    mov w8, #SYS_FORK
    svc #0
    ret

.global exit
exit:
    mov w8, #SYS_EXIT
    svc #0
    ret
