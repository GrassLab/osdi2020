.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, 2f
  
_loop:
    wfe
    b _loop

2:
    ldr x1, =_start
    mov sp, x1

    ldr x1, =__bss_start
    ldr w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       _loop
