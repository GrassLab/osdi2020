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

    // load exception table for el1
    ldr     x0, =vectors
    msr     vbar_el1, x0

    // el0 with interrupt enabled
    ldr     x0, 0
    msr     spsr_el1, x0

    adr     x0, el0_entry
    msr     elr_el1, x0

    // from el1 to el0
    eret

el0_entry:
    mov     sp, #LOW_MEMORY

kernel:
    bl      main
    b       halt
