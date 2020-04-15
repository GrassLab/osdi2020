#include "sysregs.h"

.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // set stack before our code
    ldr     x1, =_start

    msr     sp_el1, x1

    // hcr_el2 to AArch64
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0

    // spsr_el2
    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0

    // exception tables
    ldr     x2, =el2_exception_table
    msr     vbar_el2, x2

    ldr     x2, =el1_exception_table
    msr     vbar_el1, x2

    // jump to el1_start
    adr     x0, el1_start
    msr     elr_el2, x0
    eret

el1_start:
    // change exception level to EL0
    mov     x0, USER_STACK
    msr     sp_el0, x0
    mov     x0, 0 // EL0 with interrupt enabled
    msr     spsr_el1, x0
    adr	    x0, el0_start
	msr	    elr_el1, x0
    eret

el0_start:
    // mov     sp, x1 
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b
