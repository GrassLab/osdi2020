#define THREAD_CPU_CONTEXT 0

.global switch_to
switch_to:
    mov x10, #THREAD_CPU_CONTEXT
    add x8, x0, x10
    mov x9, sp
    stp x19, x20, [x8], #16     // store callee-saved registers
    stp x21, x22, [x8], #16
    stp x23, x24, [x8], #16
    stp x25, x26, [x8], #16
    stp x27, x28, [x8], #16
    stp x29, x9, [x8], #16
    str x30, [x8]

    add x8, x1, x10
    ldp x19, x20, [x8], #16     // restore callee-saved registers
    ldp x21, x22, [x8], #16
    ldp x23, x24, [x8], #16
    ldp x25, x26, [x8], #16
    ldp x27, x28, [x8], #16
    ldp x29, x9, [x8], #16
    ldr x30, [x8]
    mov sp, x9
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
    // bl  schedule_tail
    mov x0, x20
    blr x19         //should never return

