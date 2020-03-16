.globl memzero
memzero:
	//str xzr, [x0], #8
	//subs x1, x1, #8
	//b.gt memzero
    cmp x0, x1
    str xzr, [x0], #8
    bne memzero
	ret
