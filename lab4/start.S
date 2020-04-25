.section ".text.boot"

.global _start

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
    
1:  wfe
    b 1b



2:  
    ldr x0, =exception_table
    msr vbar_el1, x0

    ldr x1, =0x180000 // init sp for el1 option 1 (kernel stack)
    msr sp_el1, x1

    mov x0, (1 << 31) // EL1 use aarch64
    msr hcr_el2, x0

    mov x0, 0x5 // EL1h (SPSel = 1) with interrupt disabled, origin 0x3c5
    msr spsr_el2, x0
    adr x0, rest_initialization // load exception return address
    msr elr_el2, x0
    eret

.global rest_initialization
rest_initialization:

    ldr x1, =__bss_start //clear bss
    ldr w2, =__bss_size
3:  cbz w2, 2f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b
2:    
    bl kernel_init
    b 1b

4:  

    ldr x0, =0x280000 //(user stack)
    msr sp_el0, x0
    mov x0, 0
    msr spsr_el1, x0
    adr x0, main
    msr elr_el1, x0
    eret
    bl main
    b 1b
    
