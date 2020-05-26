.section ".text.entry"

.globl user_entry
user_entry:
    bl main
    bl exit

.set SYS_GET_TASKID_NUMBER,  0
.set SYS_FORK_NUMBER,        1
.set SYS_EXIT_NUMBER,        2
.set SYS_EXEC_NUMBER,        3
.set SYS_UART_READ_NUMBER,   4
.set SYS_UART_WRITE_NUMBER,  5


.globl get_taskid
get_taskid:
    mov    w8, #SYS_GET_TASKID_NUMBER
    svc    #0
    ret

.globl fork
fork:
    mov    w8, #SYS_FORK_NUMBER
    svc    #0
    ret

.globl exit
exit:
    mov w8, #SYS_EXIT_NUMBER    
    svc #0
    ret

.globl exec
exec:
    mov w8, #SYS_EXEC_NUMBER
    svc #0
    ret

.globl uart_read
uart_read:
    mov w8, SYS_UART_READ_NUMBER
    svc #0
    ret

.globl uart_write
uart_write:
    mov w8, SYS_UART_WRITE_NUMBER
    svc #0
    ret

