.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.globl get_cntfrq
get_cntfrq:
    mrs x0, cntfrq_el0
    ldr w0, [x0]
    ret

.globl get_cntpct
get_cntpct:
    mrs x0, cntpct_el0
    ldr w0, [x0]
    ret
