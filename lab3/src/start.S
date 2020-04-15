#include "peripherals.h"
#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF		// Check processor id
	cbz	x0, clear_bbs		// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
	b 	proc_hang

clear_bbs:
	adr	x0, __bss_start
	adr	x1, __bss_end
	sub	x1, x1, x0
	bl 	memzero

stack_pointer__set:
	ldr x0, =__code_start
	mov	sp, x0

el2_to_el1:

//SCTLR
	// ???
	ldr     x0, =SCTLR_VALUE_MMU_DISABLED	// ???
    msr     sctlr_el1, x0					// system control register set

// SCR
	// set bit 10 of scr_el2 so that EL1 is AArch64
	// set bit 0 of scr_el2 so that EL0, EL1, EL2 are all in non-secure mode
	//ldr	x0, =SCR_VALUE
	//msr	scr_el3, x0		// secure configuration register
	//msr	scr_el2, x0

// HCR
	// set bit 31 of hcr_el2 so that the register width in EL1 is 64bits
    ldr     x0, =HCR_VALUE	// EL1 uses AArch64
    msr     hcr_el2, x0		// hypervisor configuration register set

// VBAR
	ldr     x0, =vectors
	// assign the VBAR to EL1 exception vector table
    msr     vbar_el1, x0

// SPSR
    ldr     x0, =SPSR_VALUE	// set [6:8] 111(mask Serror, IRQ, FIQ)
							// set [3:0] 0101([3:2]:ELx, [1]:???, [0]:SP_ELx)
    msr     spsr_el2, x0	// saved program status register

// ELR
    adr     x0, el1_to_el0	// jump to el1_entry
    msr     elr_el2, x0		// exception link register

    eret

el1_to_el0:

	ldr     x0, =__code_start
    mov     sp, x0

// SPSR
    // EL0 with interrupt enabled(?)
    ldr     x0, 0
    msr     spsr_el1, x0

// ELR
    adr     x0, shell
    msr     elr_el1, x0

    eret

shell:
    mov	sp, #LOW_MEMORY
	
	bl	main
	b 	proc_hang		// should never come here
