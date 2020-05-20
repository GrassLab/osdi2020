.section ".text.context"

.global getCurrentTask
getCurrentTask:
    mrs x0, tpidr_el1
    ret

.macro _save_callee_saved_registers
stp x19, x20, [x0, 16 * 0]
stp x21, x22, [x0, 16 * 1]
stp x23, x24, [x0, 16 * 2]
stp x25, x26, [x0, 16 * 3]
stp x27, x28, [x0, 16 * 4]
stp fp, lr, [x0, 16 * 5]
.endm

.macro _restore_callee_saved_registers
ldp x19, x20, [x1, 16 * 0]
ldp x21, x22, [x1, 16 * 1]
ldp x23, x24, [x1, 16 * 2]
ldp x25, x26, [x1, 16 * 3]
ldp x27, x28, [x1, 16 * 4]
ldp fp, lr, [x1, 16 * 5]
.endm

.global switchCpuContext
switchCpuContext:
    _save_callee_saved_registers

    mov x9, sp
    str x9, [x0, 16 * 6]

    // load sp, physical_pgd
    ldp x9, x10, [x1, 16 * 6]
    dsb ish // ensure write has completed
    msr TTBR0_EL1, x10
    tlbi vmalle1is // invalidate all TLB entries
    dsb ish // ensure completion of TLB invalidatation
    isb // clear pipeline

    // restore callee-saved registers only
    _restore_callee_saved_registers

    mov sp,  x9

    msr tpidr_el1, x1
    ret

.global getUserCurrentTask
getUserCurrentTask:
    mrs x0, tpidr_el0
    ret

.global initUserTaskandSwitch
initUserTaskandSwitch:
    // set up user mode context, set up lr for forever loop back to barTask
    ldp fp, lr, [x1, 16 * 0] 
    ldr x10, [x1, 16 * 1]
    msr ELR_EL1, lr
    msr SP_EL0, x10 

    mov x0, 0 // EL0 with interrupt enable
    msr SPSR_EL1, x0

    msr TPIDR_EL0, x1

    eret

.global switchToEl1
switchToEl1:
    // save user mode context
    mrs x9, SP_EL0
    mrs x10, ELR_EL1
    stp x9, x10, [x0, 16 * 1]
    mrs x9, SPSR_EL1
    str x9, [x0, 16 * 2]

    ret

.global switchToEl0
switchToEl0:
    // restore SP, ELR, SPSR from user context
    ldp x9, x10, [x1, 16 * 1]
    msr SP_EL0, x9
    msr ELR_EL1, x10
    ldr x9, [x1, 16 * 2]
    msr SPSR_EL1, x9

    msr TPIDR_EL0, x1

    ret
