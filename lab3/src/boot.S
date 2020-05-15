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

    ldr x0, =__stack_top
    mov sp, x0

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

#ifdef RUN_ON_EL2
    // note this
    bl init_irq
    ldr x0, =exception_table
    msr vbar_el2, x0
    b kernel_entry
#else
#if defined(RUN_ON_EL0) || defined(RUN_ON_EL1)
    //ldr x0, =exception_table
    //msr vbar_el2, x0

    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0

    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry
    msr	elr_el2, x0

    eret
#else
    b kernel_entry
#endif
#endif

el1_entry:

    //mov	x0, #0x200000
    //mov	sp, x0

    // need?
    ldr x0, =__stack_top
    mov sp, x0

    ldr x0, =exception_table
    msr vbar_el1, x0

    bl init_irq

#ifdef RUN_ON_EL1
    b kernel_entry
#endif

    ldr x0, 0
    msr spsr_el1, x0

    adr x0, el0_entry
    msr elr_el1, x0

    // goto el0
    eret

el0_entry:

    mov x0, #LOW_MEMORY
    mov sp, x0

kernel_entry:
    bl main
