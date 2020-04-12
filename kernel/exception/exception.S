.section ".text"

.global save_reg
.global load_reg
.global exception_table

# save general registers to stack
.macro save_reg

    sub     sp,     sp,     32 * 8
    stp     x0,     x1,     [sp ,16 * 0]
    stp     x2,     x3,     [sp ,16 * 1]
    stp     x4,     x5,     [sp ,16 * 2]
    stp     x6,     x7,     [sp ,16 * 3]
    stp     x8,     x9,     [sp ,16 * 4]
    stp     x10,    x11,    [sp ,16 * 5]
    stp     x12,    x13,    [sp ,16 * 6]
    stp     x14,    x15,    [sp ,16 * 7]
    stp     x16,    x17,    [sp ,16 * 8]
    stp     x18,    x19,    [sp ,16 * 9]
    stp     x20,    x21,    [sp ,16 * 10]
    stp     x22,    x23,    [sp ,16 * 11]
    stp     x24,    x25,    [sp ,16 * 12]
    stp     x26,    x27,    [sp ,16 * 13]
    stp     x28,    x29,    [sp ,16 * 14]
    str     x30,            [sp, 16 * 15]

.endm

# load general registers from stack
.macro load_reg

    ldp     x0,     x1,     [sp ,16 * 0]
    ldp     x2,     x3,     [sp ,16 * 1]
    ldp     x4,     x5,     [sp ,16 * 2]
    ldp     x6,     x7,     [sp ,16 * 3]
    ldp     x8,     x9,     [sp ,16 * 4]
    ldp     x10,    x11,    [sp ,16 * 5]
    ldp     x12,    x13,    [sp ,16 * 6]
    ldp     x14,    x15,    [sp ,16 * 7]
    ldp     x16,    x17,    [sp ,16 * 8]
    ldp     x18,    x19,    [sp ,16 * 9]
    ldp     x20,    x21,    [sp ,16 * 10]
    ldp     x22,    x23,    [sp ,16 * 11]
    ldp     x24,    x25,    [sp ,16 * 12]
    ldp     x26,    x27,    [sp ,16 * 13]
    ldp     x28,    x29,    [sp ,16 * 14]
    ldr     x30,            [sp, 16 * 15]
    add     sp,     sp,     32 * 8
    eret

.endm

_exception__handler:

    # ESR_EL2:  Holds syndrome information for an exception taken to EL2.
    # ELR_EL2:  When taking an exception to EL2, holds the address to return to.
    # SPSR_EL2: Holds the saved process state when an exception is taken to EL2.
    # FAR_EL2:  Holds the faulting Virtual Address for all synchronous Instruction or Data Abort, PC alignment fault and Watchpoint exceptions that are taken to EL2.

    save_reg
    mrs     x0, elr_el2
    mrs     x1, esr_el2
    # mrs     x2, spsr_el2
    # mrs     x3, far_el2
    bl      exec_controller
    load_reg

_irq__handler:
    # how to make macro gloabl?
    save_reg
    bl      irq_controller
    load_reg

# Simple vector table

# vector table should be aligned to 0x800
.align 11
exception_table:

    # ==================================================
    # Exception from the current EL whild using SP_EL0
    # ==================================================

    # Synchronous
    # branch to a handler function.
    b   _exception__handler
    # entry size is 0x80, .align will pad 0
    .align 7

    # IRQ
    b   _exception__handler
    .align 7
    
    # FIQ
    b   _exception__handler
    .align 7
    
    # SError
    b   _exception__handler
    .align 7

    # ==================================================
    # Exception from the current EL whild using SP_ELx
    # ==================================================
    # Synchronous
    b   _exception__handler
    .align 7

    # IRQ
    b   _irq__handler
    .align 7
    
    # FIQ
    b   _exception__handler
    .align 7
    
    # SError
    b   _exception__handler
    .align 7


    # ==================================================
    # Exception from a lower EL and at least one lower EL is AArch64
    # ==================================================
    # Synchronous
    b   _exception__handler
    .align 7

    # IRQ
    b   _irq__handler
    .align 7
    
    # FIQ
    b   _exception__handler
    .align 7
    
    # SError
    b   _exception__handler
    .align 7

    # ==================================================
    # Exception from a lower EL and at least one lower EL is AArch32
    # ==================================================
    # Synchronous
    b   _exception__handler
    .align 7

    # IRQ
    b   _exception__handler
    .align 7
    
    # FIQ
    b   _exception__handler
    .align 7
    
    # SError
    b   _exception__handler
    .align 7
