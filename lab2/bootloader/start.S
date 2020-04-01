.section ".text.boot"

.global _start
_start:
    // save arguments in registers (we will need them later for the new kernel)
    // Choose x10-x13 because instructions generated from C by gcc does not touch them.
    mov     x10, x0
    mov     x11, x1
    mov     x12, x2
    mov     x13, x3

    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1          // get cpuid
    and     x1, x1, #3             // x0 = x0 & 3
    cbz     x1, 2f                 // jump if cpuid!=0

loop:
    wfe                            // wait for event
    b loop                         // buzy loop

/* set stack pointer and branch to main function. */
2:
    // relocate our code from load address to link address
    ldr     x1, =0x80000
    ldr     x2, =_start
    ldr     w3, =__loader_size

1:
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, 1b

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

set_zero:
    cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, set_zero

4:  
    // jump to relocated C code, should not return
    bl      main-2048
    // for failsafe, halt this core too
    b       loop
