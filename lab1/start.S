.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    // bit mask for cpu id
    and     x1, x1, #3
    cbz     x1, 2f

1:  // if cpu id is not zero, enter infinite loop
    wfe
    b       1b
2:  // if cpu id is zero
    // set stack before code
    ldr     x1, = _start
    mov     sp, x1

    // clear bss
    ldr     x1, = __bss_start
    ldr     w2, = __bss_size
3:  // if __bss_size is zero
    cbz     w2, 4f
    // move xzr(64-bits zero reg) to x1 addr,
    // and x1 shift 8
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jmp to C code, shouldn't return
    // if failsafe, halt this core
4:  bl      main
    b       1b
    
