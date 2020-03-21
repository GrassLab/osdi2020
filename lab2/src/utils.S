.globl put32
put32:
    str w1,[x0]
    ret

.global get32
get32:
    ldr w0, [x0]
    ret

.global delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.global branch_to_address
branch_to_address:
    br x0
