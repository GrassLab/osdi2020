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

.globl set_pgd
set_pgd:
	msr	ttbr0_el1, x0
	tlbi vmalle1is // invalidate all TLB entries
 	dsb ish        // ensure completion of TLB invalidatation
	isb            // clear pipeline
	ret
