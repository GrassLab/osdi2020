#include "arm/sysregs.h"
#include "mm.h"

.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, _main
    b _loop

_main:
    // top level control at el0, el1; sysregs.h:16
    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0
    
    // HCR_RW
    ldr x0, =HCR_VALUE
    msr hcr_el2, x0
    
    // Holds the saved process state when an exception happened on EL2
    // mask A, I, F and select stack ptr to EL1h
    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0
    
    adr x0, el1_entry
    msr elr_el2, x0
    
    eret

el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero
    mov sp, #LOW_MEMORY
    
    bl uart_init
    bl init_el1_vector
    bl kernel_main
    b _loop

_loop:
    b _loop
