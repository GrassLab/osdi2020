.macro SYSCALL_DEFINE name number
  .global \name
  \name:
    mov x8, #\number
    svc #0
    ret
.endm

SYSCALL_DEFINE get_taskid 0
