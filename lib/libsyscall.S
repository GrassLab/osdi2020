.macro SYSCALL_DEFINE name number
  .global \name
  \name:
    mov x8, #\number
    svc #0
    ret
.endm

SYSCALL_DEFINE get_taskid 0
SYSCALL_DEFINE uart_read 1
SYSCALL_DEFINE uart_write 2
SYSCALL_DEFINE exec 3
