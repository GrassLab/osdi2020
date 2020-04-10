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

// .macro ISR num
//     bl      __exception_store_context
//     mov     x0, \num
//     bl      exc_handler
//     bl      __exception_load_context
//     eret
// .endm

.macro EXCEPTION_ROUTINE num
    // sub    sp,  sp,  #16 * 18
    // stp    x0,  x1,  [sp, #16 * 0]
    // mov    x0, \num
    b      __exception_store_context
.endm
__exception_store_context:
    // Make room on the stack for the exception context.
    sub    sp,  sp,  #16 * 18

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

    //mrs     x2, elr_el2
    //mrs     x3, spsr_el2

    // store PC and PSTATE
    //stp     lr,  x2,  [sp, #16 * 16]
    //str     w3,       [sp, #16 * 17]

    mov     x0, #0
    bl      exc_handler

__exception_load_context:
    // load PC and PSTATE
    //ldr    w19,      [sp, #16 * 17]
    //ldp    lr,  x20, [sp, #16 * 16]

    //msr    spsr_el2, x19
    //msr    elr_el2,  x20

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

    add    sp,  sp,  #16 * 17
    
    eret


__interrupt_handler:
    bl      disable_irq
    //EXCEPTION_ROUTINE #
    bl      debug
    bl      core_timer_handler
    //bl      local_timer_handler
after_timer:
    bl      enable_irq
    eret
    

// #define HCR_RW	    (1 << 4)
// #define HCR_VALUE	HCR_RW

// .global set_HCR_EL2_IMO
// set_HCR_EL2_IMO:
//     ldr	    x0, =HCR_VALUE
// 	msr	    hcr_el2, x0
//     ret

// #define CORE0_TIMER_IRQ_CTRL 0x40000040

// .global core_timer_enable
// core_timer_enable:
//     mov     x0, 1
//     msr     cntp_ctl_el0, x0 // enable timer
//     mov     x0, 2
//     ldr     x1, =CORE0_TIMER_IRQ_CTRL
//     str     x0, [x1] // enable timer interrupt
//     ret

#define EXPIRE_PERIOD 0xffffff

.global core_timer_handler
core_timer_handler:
    mov     x0, EXPIRE_PERIOD
    msr     cntp_tval_el0, x0
    bl      core_timer_counter
    b       after_timer












.align 11
.global _exception_table
_exception_table:
// Current exception level with SP_EL0.
    // synchronous
    .align 7 //  0x000
    EXCEPTION_ROUTINE #0

    // IRQ
    .align 7 //  0x080
    EXCEPTION_ROUTINE #1

    // FIQ
    .align 7 //  0x100
    EXCEPTION_ROUTINE #2

    // SError
    .align 7 //  0x180
    EXCEPTION_ROUTINE #3

    // Current exception level with SP_ELx, x > 0.
    // synchronous
    .align 7 //  0x200
    EXCEPTION_ROUTINE #4

    // IRQ
    .align 7 //  0x280
    //EXCEPTION_ROUTINE #5
    b   __interrupt_handler

    // FIQ
    .align 7 //  0x300
    EXCEPTION_ROUTINE #6

    // SError
    .align 7 //  0x380
    EXCEPTION_ROUTINE #7

    // Lower exception level, AArch64
    // synchronous
    .align 7 //  0x400
    EXCEPTION_ROUTINE #8

    // IRQ
    .align 7 //  0x480
    EXCEPTION_ROUTINE #9

    // FIQ
    .align 7 //  0x500
    EXCEPTION_ROUTINE #10

    // SError
    .align 7 //  0x580
    EXCEPTION_ROUTINE #11

    // Lower exception level, AArch32
    // synchronous
    .align 7 //  0x600
    EXCEPTION_ROUTINE #12

    // IRQ
    .align 7 //  0x680
    EXCEPTION_ROUTINE #13

    // FIQ
    .align 7 //  0x700
    EXCEPTION_ROUTINE #14

    // SError
    .align 7 //  0x780
    EXCEPTION_ROUTINE #15

    .align 7 //  0x800

