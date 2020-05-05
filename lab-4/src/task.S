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

    // save user context
    mrs	x20, sp_el0
    mrs	x21, elr_el1
	mrs	x22, spsr_el1
    str x20, [x0, 8 * 13]
    str x21, [x0, 8 * 14]
    str x22, [x0, 8 * 15]

    // restore user context
    ldr x20, [x1, 8 * 13]
    ldr x21, [x1, 8 * 14]
    ldr x22, [x1, 8 * 15]
    msr	sp_el0, x20
    msr	elr_el1, x21 
	msr	spsr_el1, x22 

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

.globl switch_exit
switch_exit:
    mov    x0, x20
    blr    x19

.globl fork_child_exit
fork_child_exit:
    nop