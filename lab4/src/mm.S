.globl memzero
memzero:
    cmp x0, x1
    str xzr, [x0], #8
    bne memzero
	ret
