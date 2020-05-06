.macro save_all
    sub sp, sp, 34 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]

    mrs x0, SP_EL0
    mrs x1, ELR_EL1
    mrs x2, SPSR_EL1
    stp x30, x0, [sp ,16 * 15]
    stp x1, x2, [sp ,16 * 16]

.endm

.macro load_all
    ldp x30, x0, [sp ,16 * 15]
    ldp x1, x2, [sp ,16 * 16]
    msr SP_EL0, x0
    msr ELR_EL1, x1
    msr SPSR_EL1, x2

    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    add sp, sp, 34 * 8
    eret
.endm

.global test_handler
test_handler:
    save_all
    bl handler
    load_all

.global el1_irq_handler
el1_irq_handler:
    save_all
    bl el1_irq_isr
    bl check_resched_flag
    load_all

.global el0_irq_handler
el0_irq_handler:
    save_all
    bl el0_irq_isr
    bl check_resched_flag
    load_all
