// refs. https://github.com/bztsrc/raspi3-tutorial/

.section ".text.boot"

.global _start

_start:
    mrs     x1, mpidr_el1       // read cpu id from Multiprocessor Affinity Register(mpidr_el1)
    and     x1, x1, #3
    cbz     x1, 2f              // branch to label 2 if cpuid is 0
1:  wfe
    b       1b                  // jump to self
2:
    ldr     x1, =_start         // set stack before our code
    mov     sp, x1

    ldr     x1, =__bss_start    // clear bss
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

4:  bl      main                // jump to C code, should not return
    b       1b                  // for failsafe, halt this core too