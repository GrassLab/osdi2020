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
    mov     sp, x1

    // load exception_table to VBAR_EL2
    ldr x0, =vector_table
    msr VBAR_EL2, x0

    /*//route debug exceptions(brk) to el2 
    mrs x0, MDCR_EL2
    orr x0, x0, 0x100
    msr MDCR_EL2, x0*/

    # set HCR_EL2.IMO and clear PSTATE.DAIF
    mrs x0, HCR_EL2 
    orr x0, x0, 0x10
    msr HCR_EL2 , x0
    MSR DAIFClr,  #15

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