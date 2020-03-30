.section .text.relocate
.global _relocate

// let core with cpuid != 0 enter busy loop
_relocate:
    mrs x0, mpidr_el1
    and x0, x0, 3     // Get the frist two bits in Aff0 fileds
    cbz x0, 2f       // Compare and Branch on Zero,
                      // if zero goto forward label2, otherwise continue
// cpuid > 0
.global halt
halt:
    wfe
    b halt

// cpuid = 0
2:
    ldr x0, =__bss_start
    ldr w2, =__bss_size
3:
    cbz     w2, 4f          // if the size of bss is 0 then goto label 4 forward
    str     xzr, [x0], #8   // *x0 = zero, x0 = x0 + 8
    sub     w2, w2, #1      // w2 = w2 - 1
    cbnz    w2, 3b          // go to label 3 backward, until the size of bss is 0

    ldr     x0, =__stack_top
    // ldr     x0, =__stack_top
    mov     sp, x0
    // b _start
4:
    bl      relocate

.section ".text.boot"

.global _start

_start:
    bl main
    b halt
