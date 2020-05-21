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

    // set up translation control reg (el1)
    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    bl create_page_table
    
    bl start_kernel
    b _loop

create_page_table:
    mov x0, #0              // address of PGD
    mov x1, 0x1000          // address of PUD0, =512 * 8 bytes

    ldr x2, =BOOT_PGD_ATTR
    orr x2, x1, x2          // TODO: Why no offset 2
    str x2, [x0]

    ldr x2, =BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1]            // TODO: first 512MB or 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8]         // write address of 2nd PUD's entry

    msr ttbr0_el1, x0

    mrs x2, sctlr_el1
    orr x2, x2, #1
    msr sctlr_el1, x2       // enable MMU, cache remains disabled
    ret

_loop:
    b _loop
