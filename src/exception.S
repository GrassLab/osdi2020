#include "../include/exception.h"
#include "../include/interrupt.h"

.section ".text.exception"
/// Call the function provided by parameter `\handler` after saving the exception context.
/// Provide the context as the first parameter to '\handler'.
.macro EXCEPTION_ROUTINE num
//__exception_store_context:
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
    str    w30,      [sp, #16 * 15]

    mrs     x2, elr_el2
    mrs     x3, spsr_el2

    // store PC and PSTATE
    stp     lr,  x2,  [sp, #16 * 16]
    str     w3,       [sp, #16 * 17]

    mov    x0, \num
    bl      exc_handler

//__exception_load_context:
    // load PC and PSTATE
    ldr    w19,      [sp, #16 * 17]
    ldp    lr,  x20, [sp, #16 * 16]
    ldr    w30,      [sp, #16 * 15]

    msr    spsr_el2, x19
    msr    elr_el2,  x20

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
.endm

__interrupt_handler:
    //EXCEPTION_ROUTINE #1
    bl handle_timer_irq
    eret

#define HCR_RW	    (1 << 31)
#define HCR_VALUE	HCR_RW

.global set_HCR_EL2_IMO
set_HCR_EL2_IMO:
    ldr	    x0, =HCR_VALUE
	msr	    hcr_el2, x0
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable timer
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1] // enable timer interrupt
    ret

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
    EXCEPTION_ROUTINE #0

    // IRQ
    .align 7 //  0x280
    //EXCEPTION_ROUTINE #1
    b   __interrupt_handler

    // FIQ
    .align 7 //  0x300
    EXCEPTION_ROUTINE #2

    // SError
    .align 7 //  0x380
    EXCEPTION_ROUTINE #3

    // Lower exception level, AArch64
    // synchronous
    .align 7 //  0x400
    EXCEPTION_ROUTINE #0

    // IRQ
    .align 7 //  0x480
    EXCEPTION_ROUTINE #1

    // FIQ
    .align 7 //  0x500
    EXCEPTION_ROUTINE #2

    // SError
    .align 7 //  0x580
    EXCEPTION_ROUTINE #3

    // Lower exception level, AArch32
    // synchronous
    .align 7 //  0x600
    EXCEPTION_ROUTINE #0

    // IRQ
    .align 7 //  0x680
    EXCEPTION_ROUTINE #1

    // FIQ
    .align 7 //  0x700
    EXCEPTION_ROUTINE #2

    // SError
    .align 7 //  0x780
    EXCEPTION_ROUTINE #3

    .align 7 //  0x800