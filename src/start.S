.section ".text.boot"

.global _start

_start:
    // Set slave PEs to low-power state
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f 
1:  // cpu id != 0
    wfe 
    b       1b
2:  // cpu id == 0
    // set sp before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start // 64-bit name
    ldr     w2, =__bss_size  // 32-bit name
3:  cbz     w2, 4f
    str     xzr, [x1], #8    // zero reg, imm offset
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b
