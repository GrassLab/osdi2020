.macro SYS_CALL type num
  .global \type
  \type:
    mov x8, #\num
    svc #0
    ret
.endm

SYS_CALL uart_read 0
SYS_CALL uart_write 1
SYS_CALL exec 2
SYS_CALL fork 3
SYS_CALL exit 4
