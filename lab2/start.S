.section ".text.boot"

.global _start

_start:
    mrs    x0, mpidr_el1
    and    x0, x0, #3 
    cbz    x0, 2f //if x0 == 0 jump to forward of 2

1:  wfe
    b      1b //busy loop

2:
    //if cpuid == 0, set stack 
    ldr    x0, =_start
    mov    sp, x0
    //clear bss
    ldr    x0, =__bss_start
    ldr    w1, =__bss_size

3:  //if w1 == 0,
    cbz    w1, 4f
    // move xzr to x0 addr and add x0 by 8
    // set x0 to xzr(zero) and add by 9
    str    xzr, [x0], #8
    //sub bss_size until bss_size = 0
    sub    w1, w1, #1
    cbnz   w1, 3b

    //jump to c code
4:  bl     main
    b      1b

.global branch_to_address
branch_to_address:
    br x0
    ret // We never reach this point
