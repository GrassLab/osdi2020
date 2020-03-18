# 1 "src/start.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "src/start.S"
.section ".text.boot"

.global _start

_start:

    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f

1: wfe
    b 1b
2:


    ldr x1, =_start
    mov sp, x1


    ldr x1, =__bss_start
    ldr w2, =__bss_size
3: cbz w2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b


4: bl main

    b 1b
