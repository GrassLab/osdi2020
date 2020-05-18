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
    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

    mov	x0, #VA_START
    add	sp, x0, #LOW_MEMORY


    // setup TCR
    ldr x0, =(TCR_VALUE)
    msr tcr_el1, x0

    // setup MAIR
    ldr x0, =(MAIR_VALUE)
    msr mair_el1, x0


    bl page_table_create

    // indirect branch
    ldr x3, =el1_main

    mrs x2, sctlr_el1
    orr x2, x2, 1
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
    mov x0, #VA_START
    add sp, x0, #LOW_MEMORY

    // load exception table for el1
    ldr x0, =vectors
    msr vbar_el1, x0

    // enable interrupt
    bl  enable_interrupt_controller
    msr daifclr, #0xf

    // Lab4. start from el1_main
    br  x3
    b   halt

page_table_create:
    mov x29, x30
    adrp	x0, pg_dir
    adrp	x1, pg_dir
    adrp	x2, pg_dir
    adrp	x3, pg_dir
    add x0, x0, 0x0000 // PGD's page frame at 0x0
    add x1, x1, 0x1000 // PUD's page frame at 0x1000
    add x2, x2, 0x2000 // 1st PMD's page frame at 0x2000
    add x3, x3, 0x3000 // 2nd PMD's page frame at 0x4000



    ldr x4, =LV3_PGD_ATTR
    orr x4, x1, x4 // combine the physical address of next level page with attribute.
    str x4, [x0]

    ldr x4, =LV3_PUD_ATTR
    orr x5, x2, x4 // combine the physical address of next level page with attribute.
    str x5, [x1]
    orr x5, x3, x4 // combine the physical address of next level page with attribute.
    str x5, [x1, #8]


    ldr x4, =LV3_PMD_NOR_ATTR
    mov x5, #504        // physical addr boundary
    mov x6, xzr         // physical addr counter 0 to 1023
    mov x7, x2          // entry address offset
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b

    ldr x4, =LV3_PMD_DEV_ATTR
    mov x5, #512        // physical addr boundary
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b

    // TODO
    adrp	x3, pg_dir
    add x3, x3, 0x4000 // 3nd PMD's page frame at 0x6000
    ldr x4, =LV3_PMD_DEV_ATTR
    mov x5, #512        // physical addr boundary
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b
    //


    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // load PGD to the buttom translation based register.

    mov x30, x29
    ret
