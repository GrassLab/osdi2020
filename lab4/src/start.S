#include "peripherals.h"
#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF		// Check processor id
	cbz	x0, clear_bss		// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
	b 	proc_hang

clear_bss:
	adr	x0, __bss_start
	adr	x1, __bss_end
	sub	x1, x1, x0
	bl 	memzero

stack_pointer_set:
	ldr x0, =__code_start
	mov	sp, x0

el2_to_el1:


// HCR
	// set bit 31 of hcr_el2 so that the register width in EL1 is 64bits
    ldr     x0, =HCR_VALUE	// EL1 uses AArch64
    msr     hcr_el2, x0		// hypervisor configuration register set

// VBAR
	ldr     x0, =vectors
	// assign the VBAR to EL1 exception vector table
    msr     vbar_el1, x0

// SP(EL1)
	adr x0, __code_start
	msr sp_el1, x0

// SPSR
	mov		x0, #0x5
    msr     spsr_el2, x0	// saved program status register

// ELR
    adr     x0, stay_el1	// jump to el1_entry
    msr     elr_el2, x0		// exception link register

    eret

stay_el1:

	ldr     x0, 0
    msr     spsr_el1, x0

// SP(EL0)
	adr x0, __code_start
	sub x0, x0, #0x10000
	msr sp_el0, x0


shell:
    //mov	sp, #LOW_MEMORY
	
	bl	main
	b 	proc_hang		// should never come here
