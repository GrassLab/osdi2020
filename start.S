.section ".text.boot"
.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    // cpu id == 0
    cbnz     x1, loop


//     // enable interrupt
//     msr DAIFClr, 0xf
    
//     // disable interrupt
//     msr DAIFSet, 0xf
    
//     // set up EL1
//     mrs     x0, CurrentEL
//     and     x0, x0, #12 // clear reserved bits

//     // running at EL3?
//     cmp     x0, #12
//     bne     5f
    
//     // should never be executed, just for completeness
//     mov     x2, #0x5b1
//     msr     scr_el3, x2
//     mov     x2, #0x3c9
//     msr     spsr_el3, x2
//     adr     x2, 5f
//     msr     elr_el3, x2
//     eret

//     // running at EL2?
// 5:  cmp     x0, #4
//     beq     5f
//     msr     sp_el1, x1
    
//     // enable CNTP for EL1
//     mrs     x0, cnthctl_el2
//     orr     x0, x0, #3
//     msr     cnthctl_el2, x0
//     msr     cntvoff_el2, xzr
    
//     // enable AArch64 in EL1
//     mov     x0, #(1 << 31)      // AArch64
//     orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
//     msr     hcr_el2, x0
//     mrs     x0, hcr_el2
    
//     // Setup SCTLR access
//     mov     x2, #0x0800
//     movk    x2, #0x30d0, lsl #16
//     msr     sctlr_el1, x2
    
//     // set up exception handlers
//     ldr     x2, =_exception_table
//     msr     vbar_el1, x2

//     // change execution level to EL1
//     mov     x2, #0x3c4
//     msr     spsr_el2, x2
//     adr     x2, 5f
//     msr     elr_el2, x2
//     eret

    


    // set stack before our code
    ldr     x1, =_start
5:  mov     sp, x1

    // load exception_table to vbar_el2
    ldr     x0, =_exception_table
    msr     vbar_el2, x0

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
.macro CALL_WITH_CONTEXT //num handler
    bl __exception_store_context
.endm

.global __exception_store_context
__exception_store_context:
    // Make room on the stack for the exception context.
    sub    sp,  sp,  #16 * 17

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

    // mrs    x2,  elr_el2
    // mrs    x3,  spsr_el2

    // stp    lr,  x2,  [sp, #16 * 15]
    // str    w3,       [sp, #16 * 16]

    mov     x0, #0 //\num 
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    //mrs     x5, CurrentEL
    bl      exc_handler//\handler
    bl      __exception_load_context
    

.global __exception_load_context
__exception_load_context:
    // ldr    w19,      [sp, #16 * 16]
    // ldp    lr,  x20, [sp, #16 * 15]

    // msr    spsr_el2, x19
    // msr    elr_el2,  x20

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

    add    sp,  sp,  #16 * 17
    
    eret


.align 11
.global _exception_table
_exception_table:
// Current exception level with SP_EL0.
    // synchronous
    .align 7 //  0x000
    bl __exception_store_context
    //CALL_WITH_CONTEXT #0 exc_handler

    // IRQ
    .align 7 //  0x080
    bl __exception_store_context
    //CALL_WITH_CONTEXT #1 exc_handler

    // FIQ
    .align 7 //  0x100
    bl __exception_store_context
    //CALL_WITH_CONTEXT #2 exc_handler

    // SError
    .align 7 //  0x180
    bl __exception_store_context
    //CALL_WITH_CONTEXT #3 exc_handler

    // Current exception level with SP_ELx, x > 0.
    // synchronous
    .align 7 //  0x200
    bl __exception_store_context
    //CALL_WITH_CONTEXT #0 exc_handler

    // IRQ
    .align 7 //  0x280
    bl __exception_store_context
    //CALL_WITH_CONTEXT #1 exc_handler

    // FIQ
    .align 7 //  0x300
    bl __exception_store_context
    //CALL_WITH_CONTEXT #2 exc_handler

    // SError
    .align 7 //  0x380
    bl __exception_store_context
    //CALL_WITH_CONTEXT #3 exc_handler

    // Lower exception level, AArch64
    // synchronous
    .align 7 //  0x400
    bl __exception_store_context
    //CALL_WITH_CONTEXT #0 exc_handler

    // IRQ
    .align 7 //  0x480
    bl __exception_store_context
    //CALL_WITH_CONTEXT #1 exc_handler

    // FIQ
    .align 7 //  0x500
    bl __exception_store_context
    //CALL_WITH_CONTEXT #2 exc_handler

    // SError
    .align 7 //  0x580
    bl __exception_store_context
    //CALL_WITH_CONTEXT #3 exc_handler

    // Lower exception level, AArch32
    // synchronous
    .align 7 //  0x600
    bl __exception_store_context
    //CALL_WITH_CONTEXT #0 exc_handler

    // IRQ
    .align 7 //  0x680
    bl __exception_store_context
    //CALL_WITH_CONTEXT #1 exc_handler

    // FIQ
    .align 7 //  0x700
    bl __exception_store_context
    //CALL_WITH_CONTEXT #2 exc_handler

    // SError
    .align 7 //  0x780
    bl __exception_store_context
    //CALL_WITH_CONTEXT #3 exc_handler

    .align 7 //  0x800

