.section ".text"

_start:
    // let core with cpuid != 0 enter busy loop
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
    // clear the .bss segment to all zeros
    ldr x1, = __bss_start
    ldr x2, = __bss_end
    mov x3, #0
bssloop:
    cmp x1, x2
    bge bssdone
    str x3, [x1], #8
    b bssloop
bssdone:
    // set stack pointer and branch to main function.
    ldr x0, = __stack_start
    mov sp, x0
    //bl main
    b 1b