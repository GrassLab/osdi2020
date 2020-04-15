.section ".text"

.global _start

_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
debug:
clear_bss:
    ldr x1, =__bss_start
    ldr w2, =__bss_size
clear_bss_c:
    cbz w2, 2f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, clear_bss_c
start_el:
2:
    mov x0, #0x80000
    mov sp, x0
    //enable EL2 CPU interrupt
    //mrs x0, HCR_EL2
    //orr x0, x0, 16
    //msr HCR_EL2, x0

    //enable CNTP timer
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2

//    mrs x0, HCR_EL2
//    orr x0, x0, 0x80000000
//    msr HCR_EL2, x0
//    mov x0, xzr
//    msr DAIF, x0
    // load exception_table
    //ldr x0, =exception_table_el3
    //msr VBAR_EL3, x0

    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2

    ldr x0, =exception_table_el2
    msr VBAR_EL2, x0
    ldr x0, =exception_table_el1
    msr VBAR_EL1, x0
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, 3f
    msr     elr_el2, x2
    eret
3:  mov x0, #0x80000
    sub x0, x0, #0x1000
    mov sp, x0
    mov     x2, #0x3c5
    msr     spsr_el1, x2
    adr     x2, 4f
    msr     elr_el1, x2
    eret

4:
    mov x0, #0x80000
    sub x0, x0, #0x2000
    mov sp, x0
    MSR DAIFClr, #0xF
    bl main
//halt core for debugging
    b 1b


// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table_el3:
    .align 7
    b exc_handler_syn_el3 // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el3
    .align 7
    b exc_handler_syn_el3
    .align 7

    b exc_handler_syn_el3
    .align 7
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el3
    .align 7
    b exc_handler_syn_el3
    .align 7
exception_table_el2:
    b exc_handler_syn_el2
    .align 7
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el2
    .align 7
    b exc_handler_syn_el2
    .align 7

    b exc_handler_syn_el2
    .align 7
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el2
    .align 7
    b exc_handler_syn_el2
    .align 7
exception_table_el1:
    b exc_handler_syn_el1
    .align 7
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el1
    .align 7
    b exc_handler_syn_el1
    .align 7

    b exc_handler_syn_el1
    .align 7
    b exc_handler_irq
    .align 7
    b exc_handler_syn_el1
    .align 7
    b exc_handler_syn_el1
    .align 7

/*
.align 4
iss_table:
    .align 4
    bl require
    ret
    .align 4
    b require
    ret
    .align
*/
exc_handler_syn_el3:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x30, xzr, [sp, #-16]!

    bl require_el3

    ldp x30, xzr, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x6, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret


exc_handler_syn_el2:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x30, xzr, [sp, #-16]!

    bl require_el2

    ldp x30, xzr, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x6, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret

exc_handler_syn_el1:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x30, xzr, [sp, #-16]!

    bl require_el1

    ldp x30, xzr, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x6, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret


exc_handler_irq:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x30, xzr, [sp, #-16]!

    bl timer_irq

    ldp x30, xzr, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x6, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret