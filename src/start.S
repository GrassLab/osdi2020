.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0



el2_entry:
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    msr     sp_el1, x1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, el1_entry
    msr     elr_el2, x2

    eret

el1_entry:  
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el1, x2

    ldr     x0, 0
    msr     spsr_el1, x0

    adr     x0, el0_entry
    msr     elr_el1, x0
    eret

el0_entry:
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // load exception_table to VBAR_EL2
    // ldr x0, =exception_table
    // msr VBAR_EL2, x0

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b

