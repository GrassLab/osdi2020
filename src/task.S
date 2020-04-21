.section ".text"
.global switch_to
switch_to:
    mov x9,  sp
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp x29, x9,  [x0, 16 * 5]
    str x30,      [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp x29, x9,  [x1, 16 * 5]
    ldr x30,      [x1, 16 * 6]
    mov sp,  x9
    msr tpidr_el1, x1   // set_current
    ret

.global get_current
get_current:
    mrs x0, tpidr_el1
    ret

.global set_current
set_current:
    msr tpidr_el1, x0
    ret

.globl ret_from_fork
ret_from_fork:
    // bl    schedule_tail
    mov    x0, x20
    blr    x19         //should never return