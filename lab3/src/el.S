#define STACK_SIZE 0x10000

.global el2_to_el0
el2_to_el0:
    // switch to el1
    ldr     x1, =_start
    msr     sp_el1, x1
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // set up exception handlers
    ldr     x2, =vector_table
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x5          // EL1h and clear DAIF
    msr     spsr_el2, x2
    adr     x2, 1f
    msr     elr_el2, x2
    eret

1:  // switch to el0
    adr x0, _start
    sub x0, x0, #STACK_SIZE
    msr sp_el0, x0
    mov x0, 0x0
    msr spsr_el1, x0
    mov x0, x30
    msr elr_el1, x0
    eret