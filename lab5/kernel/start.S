#include "mm.h"
#include "sysregs.h"

.section ".text.boot"


// let core with cpuid != 0 enter busy loop
.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f

// cpuid > 0
.global halt
halt:
    wfe
    b halt

// cpuid = 0
2:
    ldr x0, =__stack_top
    mov sp, x0

    // setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    // setup MAIR
    ldr x0, =MAIR_CONFIG_DEFAULT
    msr mair_el1, x0

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
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

boot_rest:
    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry
    msr	elr_el2, x0

    eret

el1_entry:
    ldr x0, =__stack_top
    mov sp, x0

    // load exception table for el1
    ldr x0, =vectors
    msr vbar_el1, x0

    // enable interrupt
    bl  enable_interrupt_controller
    msr daifclr, #0xf

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

    // Lab4. start from el1_main
    bl      el1_main
    b       halt
