.section ".text"

.global uart_write
.global uart_read

.macro SYS_CALL_UART_WRITE
    svc     #13
.endm

.macro SYS_CALL_UART_READ
    svc     #14
.endm

uart_write:
    SYS_CALL_UART_WRITE
    ret

uart_read:
    SYS_CALL_UART_READ
    ret