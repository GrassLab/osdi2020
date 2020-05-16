.globl memcpy
memcpy:
    ldrb w3, [x1], #1
    strb w3, [x0], #1
    sub w2, w2, #1
    cbnz  w2, memcpy
    ret

.globl memzero
memzero:
    strb wzr, [x0], #1
    sub w1, w1, #1
    cbnz  w1, memzero
    ret
