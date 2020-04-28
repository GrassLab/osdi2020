.section ".text"
.global switch_to
switch_to:
    // update task state to context switch
    mov x2, #5  // CONTEXT_SWITCH
    str x2, [x0, 8 * 16]

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

    mov x2, #5  // CONTEXT_SWITCH
    ldr x2, [x1, 8 * 16]

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


#define USER_STACK 0x1000

.global _setup_user_content
_setup_user_content:
    // setup EL0 stack
    mov     x1, USER_STACK
    msr     sp_el0, x1
    // switch to EL0
    // spsr_el1 clear daif and switch to EL0
    mov     x2, 0x0
    msr     spsr_el1, x2
    // from el1 return to el0
    msr     elr_el1, x0  // void(*func)()
    eret


// .global _el0_to_el1
// _el0_to_el1:
//     // setup EL1 stack
//     ldr     x1, =_start
//     msr     sp_el1, x1
//     // switch to EL0
//     // spsr_el1 clear daif and switch to EL0
//     mov     x0, 0x5//#0x3c4
//     msr     spsr_el2, x0
//     adr     x0, _run_main  // from el2 return to el1
//     msr     elr_el2, x0
//     eret