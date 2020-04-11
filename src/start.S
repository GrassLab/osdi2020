.section ".text.boot"
.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    // cpu id == 0
    cbnz     x1, loop

    // set stack before our code
    ldr     x1, =_start

_setup_el1:
    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits //lsr x0, x0, #2


    // running at EL2?
5:  cmp     x0, #4
    beq     5f
    msr     sp_el1, x1
    
    // // enable CNTP for EL1
    // mrs     x0, cnthctl_el2
    // orr     x0, x0, #3
    // msr     cnthctl_el2, x0
    // msr     cntvoff_el2, xzr
    
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    
    // // Setup SCTLR access
    // mov     x2, #0x0800
    // movk    x2, #0x30d0, lsl #16
    // msr     sctlr_el1, x2
    
    // set up exception handlers
    ldr     x2, =_exception_table
    msr     vbar_el1, x2

    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret

    
_setup_el2:
    // load exception_table to vbar_el2
    ldr     x0, =_exception_table
    msr     vbar_el2, x0

5:  mov     sp, x1


    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
clear_bss:
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss

    // jump to C code, should not return
    bl      main
loop:  
    // for failsafe, halt this core too
    // cpu id > 0, stop
    wfe
    b       loop

.section ".text.exception"
/// Call the function provided by parameter `\handler` after saving the exception context.
/// Provide the context as the first parameter to '\handler'.

.macro EXCEPTION_ROUTINE num
    b      __exception_routine
.endm

.macro INTERRUPT_ROUTINE num
    b      __interrupt_routine
.endm

.macro STORE_CONTEXT
    // Make room on the stack for the exception context.
    sub    sp,  sp,  #16 * 16

    // Store all general purpose registers on the stack.
    stp    x0,  x1,  [sp, #16 * 0]
    stp    x2,  x3,  [sp, #16 * 1]
    stp    x4,  x5,  [sp, #16 * 2]
    stp    x6,  x7,  [sp, #16 * 3]
    stp    x8,  x9,  [sp, #16 * 4]
    stp    x10, x11, [sp, #16 * 5]
    stp    x12, x13, [sp, #16 * 6]
    stp    x14, x15, [sp, #16 * 7]
    stp    x16, x17, [sp, #16 * 8]
    stp    x18, x19, [sp, #16 * 9]
    stp    x20, x21, [sp, #16 * 10]
    stp    x22, x23, [sp, #16 * 11]
    stp    x24, x25, [sp, #16 * 12]
    stp    x26, x27, [sp, #16 * 13]
    stp    x28, x29, [sp, #16 * 14]
    str    x30,      [sp, #16 * 15]
.endm

.macro LOAD_CONTEXT
    ldp    x0,  x1,  [sp, #16 * 0]
    ldp    x2,  x3,  [sp, #16 * 1]
    ldp    x4,  x5,  [sp, #16 * 2]
    ldp    x6,  x7,  [sp, #16 * 3]
    ldp    x8,  x9,  [sp, #16 * 4]
    ldp    x10, x11, [sp, #16 * 5]
    ldp    x12, x13, [sp, #16 * 6]
    ldp    x14, x15, [sp, #16 * 7]
    ldp    x16, x17, [sp, #16 * 8]
    ldp    x18, x19, [sp, #16 * 9]
    ldp    x20, x21, [sp, #16 * 10]
    ldp    x22, x23, [sp, #16 * 11]
    ldp    x24, x25, [sp, #16 * 12]
    ldp    x26, x27, [sp, #16 * 13]
    ldp    x28, x29, [sp, #16 * 14]
    ldr    x30,      [sp, #16 * 15]

    add    sp,  sp,  #16 * 16
.endm


__exception_routine:
    STORE_CONTEXT
    mov     x0, #0
    bl      exc_handler
    LOAD_CONTEXT
    eret


__interrupt_routine:
    bl      disable_irq
    STORE_CONTEXT
    //mov     x0, #1
    //bl      exc_handler
    bl      debug
    //bl      local_timer_handler
    bl      core_timer_handler
after_timer:
    LOAD_CONTEXT
    bl      enable_irq
    eret
    

.align 11
.global _exception_table
_exception_table:
    // Current exception level with SP_EL0.
    .align 7 // synchronous //  0x000
    EXCEPTION_ROUTINE #0

    .align 7 // IRQ //  0x080
    EXCEPTION_ROUTINE #1
    
    .align 7 // FIQ //  0x100
    EXCEPTION_ROUTINE #2

    .align 7 // SError //  0x180
    EXCEPTION_ROUTINE #3

    // Current exception level with SP_ELx, x > 0.
    .align 7 // synchronous //  0x200
    EXCEPTION_ROUTINE #4

    .align 7 // IRQ //  0x280
    INTERRUPT_ROUTINE #5

    .align 7 // FIQ //  0x300
    EXCEPTION_ROUTINE #6

    .align 7 // SError //  0x380
    EXCEPTION_ROUTINE #7

    // Lower exception level, AArch64
    .align 7 // synchronous //  0x400
    EXCEPTION_ROUTINE #8
    
    .align 7 // IRQ //  0x480
    EXCEPTION_ROUTINE #9

    .align 7 // FIQ //  0x500
    EXCEPTION_ROUTINE #10

    .align 7 // SError //  0x580
    EXCEPTION_ROUTINE #11

    // Lower exception level, AArch32
    .align 7 // synchronous //  0x600
    EXCEPTION_ROUTINE #12

    .align 7 // IRQ //  0x680
    EXCEPTION_ROUTINE #13

    .align 7 // FIQ //  0x700
    EXCEPTION_ROUTINE #14

    .align 7 // SError //  0x780
    EXCEPTION_ROUTINE #15

    .align 7 //  0x800
