.section ".text"
/// Call the function provided by parameter `\handler` after saving the exception context.
/// Provide the context as the first parameter to '\handler'.

.macro EXCEPTION_ROUTINE
    b      __exception_routine
.endm

.macro INTERRUPT_ROUTINE 
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
    bl      exception_handler
    LOAD_CONTEXT
    eret


__interrupt_routine:
    //bl      disable_irq
    STORE_CONTEXT
    bl      debug
    bl      interrupt_handler
    LOAD_CONTEXT
    //bl      enable_irq
    eret
    

.align 11
.global _exception_table
_exception_table:
    // Current exception level with SP_EL0.
    .align 7 // synchronous //  0x000
    b exception_not_implement

    .align 7 // IRQ //  0x080
    b exception_not_implement
    
    .align 7 // FIQ //  0x100
    b exception_not_implement

    .align 7 // SError //  0x180
    b exception_not_implement

    // Current exception level with SP_ELx, x > 0.
    .align 7 // synchronous //  0x200
    EXCEPTION_ROUTINE

    .align 7 // IRQ //  0x280
    INTERRUPT_ROUTINE

    .align 7 // FIQ //  0x300
    b exception_not_implement

    .align 7 // SError //  0x380
    b exception_not_implement

    // Lower exception level, AArch64
    .align 7 // synchronous //  0x400
    EXCEPTION_ROUTINE
    
    .align 7 // IRQ //  0x480
    INTERRUPT_ROUTINE

    .align 7 // FIQ //  0x500
    b exception_not_implement

    .align 7 // SError //  0x580
    b exception_not_implement

    // Lower exception level, AArch32
    .align 7 // synchronous //  0x600
    b exception_not_implement

    .align 7 // IRQ //  0x680
    b exception_not_implement

    .align 7 // FIQ //  0x700
    b exception_not_implement

    .align 7 // SError //  0x780
    b exception_not_implement

    .align 7 //  0x800
