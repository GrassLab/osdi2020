.section ".text.boot"
.global _start

_start:
    mrs  x1, mpidr_el1
    and  x1, x1, #3
    cbz  x1, _init

_loop:
    wfe
    b    _loop

_init:
    ldr  x1, =_start
    mov  sp, x1
    
    ldr  x1, =__bss_start
    ldr  w2, =__bss_size

_zero:
    cbz  w2, _jmain
    str  xzr, [x1], #8
    sub  w2, w2, #1
    cbnz w2, _zero

_jmain:
    bl   main
    b    _loop
