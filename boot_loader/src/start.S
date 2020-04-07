.section ".text"

.global _start

_start:
    // let core with cpuid != 0 enter busy loop
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, init
busyloop:
    wfe
    b busyloop

init:
// relocate our code from load address to link address
    ldr     x1, =0x80000
    ldr     x2, =_bootloader_start
    ldr     w3, =__loader_size
relocateloop:  
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, relocateloop
    
meminit:
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
    ldr x0, = _bootloader_start
    mov sp, x0
    bl main-2048
    b busyloop