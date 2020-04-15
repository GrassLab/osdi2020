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

    // set up exception handlers
    ldr     x0, =_vectors
    msr     VBAR_EL2, x0

ldr     x1, =_start
    mov     sp, x1

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
    
    .macro cts_s
    sub     sp, sp, #0xF8
    str     x0, [sp, #0xF0]
    str     x1, [sp, #0xE8]
    str     x2, [sp, #0xE0]
    str     x3, [sp, #0xD8]
    str     x4, [sp, #0xD0]
    str     x5, [sp, #0xC8]
    str     x6, [sp, #0xC0]
    str     x7, [sp, #0xB8]
    str     x8, [sp, #0xB0]
    str     x9, [sp, #0xA8]
    str     x10, [sp, #0xA0]
    str     x11, [sp, #0x98]
    str     x12, [sp, #0x90]
    str     x13, [sp, #0x88]
    str     x14, [sp, #0x80]
    str     x15, [sp, #0x78]
    str     x16, [sp, #0x70]
    str     x17, [sp, #0x68]
    str     x18, [sp, #0x60]
    str     x19, [sp, #0x58]
    str     x20, [sp, #0x50]
    str     x21, [sp, #0x48]
    str     x22, [sp, #0x40]
    str     x23, [sp, #0x38]
    str     x24, [sp, #0x30]
    str     x25, [sp, #0x28]
    str     x26, [sp, #0x20]
    str     x27, [sp, #0x18]
    str     x28, [sp, #0x10]
    str     x29, [sp, #0x08]
    str     x30, [sp, #0x00]
    .endm
    .macro cts_l
    ldr     x30, [sp, #0x00]
    ldr     x29, [sp, #0x08]
    ldr     x28, [sp, #0x10]
    ldr     x27, [sp, #0x18]
    ldr     x26, [sp, #0x20]
    ldr     x25, [sp, #0x28]
    ldr     x24, [sp, #0x30]
    ldr     x23, [sp, #0x38]
    ldr     x22, [sp, #0x40]
    ldr     x21, [sp, #0x48]
    ldr     x20, [sp, #0x50]
    ldr     x19, [sp, #0x58]
    ldr     x18, [sp, #0x60]
    ldr     x17, [sp, #0x68]
    ldr     x16, [sp, #0x70]
    ldr     x15, [sp, #0x78]
    ldr     x14, [sp, #0x80]
    ldr     x13, [sp, #0x88]
    ldr     x12, [sp, #0x90]
    ldr     x11, [sp, #0x98]
    ldr     x10, [sp, #0xA0]
    ldr     x9, [sp, #0xA8]
    ldr     x8, [sp, #0xB0]
    ldr     x7, [sp, #0xB8]
    ldr     x6, [sp, #0xC0]
    ldr     x5, [sp, #0xC8]
    ldr     x4, [sp, #0xD0]
    ldr     x3, [sp, #0xD8]
    ldr     x2, [sp, #0xE0]
    ldr     x1, [sp, #0xE8]
    ldr     x0, [sp, #0xF0]
    add     sp, sp, #0xF8
    .endm
    .macro EXEC_HANDLE slot, el
    //cts_s
    sub     sp, sp, #0x28
    str     x0, [sp, #0x20]
    str     x1, [sp, #0x18]
    str     x2, [sp, #0x10]
    str     x3, [sp, #0x08]
    str     x4, [sp, #0x00]
    mov     x0, 0x3C0
    msr     daif, x0
    mov     x0, \slot
    mrs     x1, esr_el\el
    mrs     x2, elr_el\el
    mrs     x3, spsr_el\el
    mrs     x4, far_el\el
    b      exc_handler
    ldr     x4, [sp, #0x00]
    ldr     x3, [sp, #0x08]
    ldr     x2, [sp, #0x10]
    ldr     x1, [sp, #0x18]
    ldr     x0, [sp, #0x20]
    add     sp, sp, #0x28
    msr     daif, xzr
    //cts_l
    eret
    .endm
    // important, code has to be properly aligned
    .align 11
.global _vectors
_vectors:
    /*
     * SP_EL0
     * ------------
     * 0x000 | Synchronous
     * 0x080 | IRQ/vIRQ
     * 0x100 | FIQ/vFIQ
     * 0x180 | SError/vError
     */
    EXEC_HANDLE 0, 1
    .align  7
    EXEC_HANDLE 1, 1
    .align  7
    EXEC_HANDLE 2, 1
    .align  7
    EXEC_HANDLE 3, 1
    .align  7
    /*
     * SP_ELx
     * ------------
     * 0x200 | Synchronous
     * 0x280 | IRQ/vIRQ
     * 0x300 | FIQ/vFIQ
     * 0x380 | SError/vError
     */
    EXEC_HANDLE 0, 2
    .align  7
    EXEC_HANDLE 1, 2
    .align  7
    EXEC_HANDLE 2, 2
    .align  7
    EXEC_HANDLE 3, 2
    .align  7
    /*
     * SP_EL???
     * ------------
     * 0x400 | Synchronous
     * 0x480 | IRQ/vIRQ
     * 0x500 | FIQ/vFIQ
     * 0x580 | SError/vError
     */
    EXEC_HANDLE 0, 3
    .align  7
    EXEC_HANDLE 1, 3
    .align  7
    EXEC_HANDLE 2, 3
    .align  7
    EXEC_HANDLE 3, 3
    .align  7
    /*
     * SP_EL???
     * ------------
     * 0x600 | Synchronous
     * 0x680 | IRQ/vIRQ
     * 0x700 | FIQ/vFIQ
     * 0x780 | SError/vError
     */
    /*
    EXEC_HANDLE 0, 4
    .align  7
    EXEC_HANDLE 1, 4
    .align  7
    EXEC_HANDLE 2, 4
    .align  7
    EXEC_HANDLE 3, 4
    .align  7
    */