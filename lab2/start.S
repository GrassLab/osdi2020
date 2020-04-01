.section ".text"
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
    ldr x0, = _start
    mov sp, x0
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
    bl      main
