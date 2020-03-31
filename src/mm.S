 .globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl memncpy
memncpy:
    ldrb w3, [x0], #1
    strb w3, [x1], #1
    sub w2, w2, #1
    cbnz  w2, memncpy
    ret

.globl memzero
memzero:
    strb wzr, [x0], #1
    sub w1, w1, #1
    cbnz  w1, memzero
    ret
