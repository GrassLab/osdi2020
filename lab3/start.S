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

5:  
    mrs     x0, hcr_el2
    // set HCR_EL2.IMO
    // orr     x0, x0, #(1 << 4)
    // set HCR_EL2.RW
    orr     x0, x0, #0x8000000
    msr     hcr_el2, x0


    // for debug not route to El1
    // mrs x0, MDCR_EL2
    // bic x0, x0, 0x100
    // msr MDCR_EL2, x0

    // clear PSTATE.DAIF
    mov     x0, #0
    msr     DAIF, x0

    // set up exception handlers
    ldr     x2, =el2_exception_table
    msr     vbar_el2, x2

    ldr     x3, =el1_exception_table
    msr     vbar_el1, x3

5:  mov     sp, x1

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



