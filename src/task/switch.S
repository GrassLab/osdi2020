.global switchTo
switchTo:
    ldp x19, x20, [x0, 16 * 0]
    ldp x21, x22, [x0, 16 * 1]
    ldp x23, x24, [x0, 16 * 2]
    ldp x25, x26, [x0, 16 * 3]
    ldp x27, x28, [x0, 16 * 4]
    ldp x29, x30, [x0, 16 * 5]
    ldr x9, [x0, 16 * 6]
    mov sp,  x9
    ldp x8, x9, [x0, 16 * 6 + 8]
    msr elr_el1, x8
    msr sp_el0, x9
    ret

.global copyAndSwitchTo
copyAndSwitchTo:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp x29, x30, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]
    mrs x8, elr_el1
    mrs x9, sp_el0
    stp x8, x9, [x0, 16 * 6 + 8]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp x29, x30, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9
    ldp x8, x9, [x1, 16 * 6 + 8]
    msr elr_el1, x8
    msr sp_el0, x9
    ret

.global switchToEL0
switchToEL0:
    ldr x8, [x0, 16 * 6] // set kernel sp
    mov sp, x8
    ldp x8, x9, [x0, 16 * 6 + 8]
    msr elr_el1, x8  // return to a function run in EL0
    msr sp_el0, x9   // set user sp
    mov x0, 0 // EL0 with interrupt enabled
    msr spsr_el1, x0 
    eret

.global copyStack
copyStack:  
    ldr x4, [x1, -8]!
    str x4, [x0, -8]!
    sub x2, x2, 8
    cbnz x2, copyStack
    ret

.global copyContext
copyContext:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp x29, x30, [x0, 16 * 5]
    mrs x8, elr_el1
    str x8, [x0, 16 * 6 + 8]
    ret