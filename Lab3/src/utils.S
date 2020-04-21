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

.global get_timer_freq
get_timer_freq:
	mrs x0, cntfrq_el0
	ret

.global get_timer_counts
get_timer_counts:
	mrs x0, cntpct_el0
	ret

.global get_reg
get_reg: 
        mov x0, SP
	ret

