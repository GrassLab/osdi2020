#include "register.h"
#include "memory.h"

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

	// bl  paging_setup

	// mov	x0, #VA_START	
	mov	sp, #LOW_MEMORY

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

paging_setup:
	// setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1,x0

    // setup MAIR
    ldr	x0, = MAIR_CONFIG_DEFAULT
    msr	mair_el1, x0

	mov x0, 0 // PGD's page frame at 0x0
	mov x1, 0x1000 // PUD's page frame at 0x1000

	ldr x2, = BOOT_PGD_ATTR
	orr x2, x1, x2 // combine the physical address of next level page with attribute.
	str x2, [x0]

	ldr x2, = BOOT_PUD_ATTR
	mov x3, 0x00000000
	orr x3, x2, x3
	str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
	mov x3, 0x40000000
	orr x3, x2, x3
	str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

	msr ttbr0_el1, x0 // load PGD to the buttom translation based register.

	mrs x2, sctlr_el1
	orr x2 , x2, 1
	msr sctlr_el1, x2 // enable MMU, cache remains disabled
	ret