#include "register.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0,#0xFF		// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
	b 	proc_hang

master:
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0		

	ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0

	ldr	x0, =SPSR_VALUE
	msr	spsr_el2, x0

	adr	x0, el1_start		
	msr	elr_el2, x0
	eret				

el1_start:
	ldr	x0, =__bss_start
    ldr x1, =__bss_size
	bl 	memzero
	mov	sp, #EL1_LOW_MEMORY

	adr x0, exception_table		// load VBAR_EL1 with virtual
	msr	vbar_el1, x0		// vector table address

	bl	main
	b 	proc_hang		// should never come here

el0_start:
	mov	sp, #EL0_LOW_MEMORY
	bl	main
	b 	proc_hang		// should never come here

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret