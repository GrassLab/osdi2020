.section ".text"

.global wait_msec

.macro SYS_CALL_WAIT_MSEC
    svc     #10
.endm

wait_msec:
    SYS_CALL_WAIT_MSEC
    ret