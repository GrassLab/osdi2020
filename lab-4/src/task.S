.section ".text"
.global switch_to
switch_to:
    mov x9,  sp
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp x29, x9,  [x0, 16 * 5] // fp, sp
    str x30,      [x0, 16 * 6] // lr

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp x29, x9,  [x1, 16 * 5] // fp, sp
    ldr x30,      [x1, 16 * 6] // lr
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

.global switch_exit
switch_exit:
    mov    x0, x20
    blr    x19

.global fork_child_exit
fork_child_exit:
    mrs x1, tpidr_el1
    ldr x0, [x1, 8 * 11]
    mov sp, x0
    ldr x21, [x1, 8 * 13]
    ldr x22, [x1, 8 * 14]
    ldr x23, [x1, 8 * 15]
    msr	sp_el0, x21
    msr	elr_el1, x22
    msr	spsr_el1, x23

    ldr x29, [x1, 8 * 10]

    ldp	x0, x1, [sp, #16 * 0]
    ldp	x2, x3, [sp, #16 * 1]
    ldp	x4, x5, [sp, #16 * 2]
    ldp	x6, x7, [sp, #16 * 3]
    ldp	x8, x9, [sp, #16 * 4]
    ldp	x10, x11, [sp, #16 * 5]
    ldp	x12, x13, [sp, #16 * 6]
    ldp	x14, x15, [sp, #16 * 7]
    ldp	x16, x17, [sp, #16 * 8]
    ldp	x18, x19, [sp, #16 * 9]
    ldp	x20, x21, [sp, #16 * 10]
    ldp	x22, x23, [sp, #16 * 11]
    ldp	x24, x25, [sp, #16 * 12]
    ldp	x26, x27, [sp, #16 * 13]
    ldr x28, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15] 
    add	sp, sp, #280
    mov x0, 0 // fork child
    eret