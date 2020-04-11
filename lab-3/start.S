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

5:  // set HCR_EL2.IMO
    mrs     x0, hcr_el2
    orr     x0, x0, #(1 << 4)
    msr     hcr_el2, x0

    // clear PSTATE.DAIF
    mov     x0, #0
    msr     DAIF, x0

    // set up exception handlers
    ldr     x2, =_vectors
    msr     vbar_el2, x2

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

    // important, code has to be properly aligned
    .align 11
_vectors:
    mov     x0, #10
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b exception_handler
    .align 7
    mov     x0, #11
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b exception_handler
    .align 7
    mov     x0, #12
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b exception_handler
    .align 7
    mov     x0, #13
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b exception_handler

    // synchronous
    .align  7
    mov     x0, #0
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b       exception_handler

    // IRQ
    .align  7
    mov     x0, #1
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b       core_timer_handler

    // FIQ
    .align  7
    mov     x0, #2
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b       exception_handler

    // SError
    .align  7
    mov     x0, #3
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    b       exception_handler