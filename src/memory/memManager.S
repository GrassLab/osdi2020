.global memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret

.globl memcpy
memcpy:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	b.gt memcpy
	ret

.globl setPgd
setPgd:
	msr ttbr0_el1, x0 // switch translation based address.
	tlbi vmalle1is // invalidate all TLB entries
	dsb ish // ensure completion of TLB invalidatation
	isb // clear pipeline
	ret
