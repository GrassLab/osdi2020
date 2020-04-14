#include "mm.h"
#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, continue

hang:
    wfe
    b hang

continue:

    ldr     x0, =__stack_top
    mov     sp, x0

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

#ifdef RUN_ON_EL2
    // note this
    bl init_interrupt
    ldr x0, =exception_table
    msr vbar_el2, x0
    b kernel_entry
#else
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0		

    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry		
    msr	elr_el2, x0

    eret
#endif

el1_entry:

    // need?
    ldr     x0, =__stack_top
    mov     sp, x0

    ldr x0, =exception_table
    msr vbar_el1, x0

#ifdef RUN_ON_EL1
    bl init_interrupt
    b kernel_entry
#endif

    ldr     x0, 0
    msr     spsr_el1, x0

    adr     x0, el0_entry
    msr     elr_el1, x0

    // goto el0
    eret

el0_entry:
    // set stack
    //ldr x0, #LOW_MEMORY
    mov x0, #LOW_MEMORY
    mov sp, x0 

kernel_entry: 
    bl main
