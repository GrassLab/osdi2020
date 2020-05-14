#include "sysregs.h"
#include "mm.h"

.section ".text.boot"
.global _start

// let core with cpuid != 0 enter busy loop
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3     // Get the frist two bits in Aff0 fileds
    cbz x0, 2f       // Compare and Branch on Zero,
                      // if zero goto forward label2, otherwise continue
// cpuid > 0
.global halt
halt:
    wfe
    b halt

// cpuid = 0
2:
    ldr x0, =__bss_start
    ldr w2, =__bss_size
3:
    cbz     w2, 4f          // if the size of bss is 0 then goto label 4 forward
    str     xzr, [x0], #8   // *x0 = zero, x0 = x0 + 8
    sub     w2, w2, #1      // w2 = w2 - 1
    cbnz    w2, 3b          // go to label 3 backward, until the size of bss is 0

4:
    ldr     x0, =__stack_top
    mov     sp, x0

el2_entry:
    // setup the exception configuration
    // system control register
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x0

    // hypervisor configuration register
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0

    // saved program status register
    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0

    adr     x0, el1_entry // load excpetion return address
    msr     elr_el2, x0

    // from el2 to el1
    eret

el1_entry:
    ldr     x0, =__stack_top
    mov     sp, x0

    // setup tcr
    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    // setup mair
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

    // Map
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD


    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled


    // load exception table for el1
    ldr     x0, =vectors
    msr     vbar_el1, x0

    // enable interrupt
    bl      enable_interrupt_controller
    msr     daifclr, #0xf

    // el0 with interrupt enabled
    ldr     x0, 0
    msr     spsr_el1, x0

    // Lab4. start from el1_main
    bl      el1_main
    b       halt



    adr     x0, el0_entry
    msr     elr_el1, x0


    // from el1 to el0
    eret

el0_entry:
    mov     sp, #LOW_MEMORY

kernel:
    bl      main
    b       halt
