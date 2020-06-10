#define S_FRAME_SIZE 16*17
#define EL0 0
#define ELx 1

.macro	ventry	label
    .align	7
    b	\label
.endm

//save trapframe
.macro  kernel_entry el
    sub	sp,  sp,  #S_FRAME_SIZE
    stp	x0,  x1,  [sp, #16 * 0]
    stp	x2,  x3,  [sp, #16 * 1]
    stp	x4,  x5,  [sp, #16 * 2]
    stp	x6,  x7,  [sp, #16 * 3]
    stp	x8,  x9,  [sp, #16 * 4]
    stp	x10, x11, [sp, #16 * 5]
    stp	x12, x13, [sp, #16 * 6]
    stp	x14, x15, [sp, #16 * 7]
    stp	x16, x17, [sp, #16 * 8]
    stp	x18, x19, [sp, #16 * 9]
    stp	x20, x21, [sp, #16 * 10]
    stp	x22, x23, [sp, #16 * 11]
    stp	x24, x25, [sp, #16 * 12]
    stp	x26, x27, [sp, #16 * 13]
    stp	x28, x29, [sp, #16 * 14]

    .if \el == EL0
        mrs x21, sp_el0
    .else
        add x21, sp, #S_FRAME_SIZE //flush stack, not used?
    .endif /* \el == 0 */

    // for nested exception
    mrs	x22, elr_el1
    mrs	x23, spsr_el1

    stp	x30, x21, [sp, #16 * 15] 
    stp	x22, x23, [sp, #16 * 16]
.endm

//load trapframe
.macro  kernel_exit el
    ldp	x22, x23, [sp, #16 * 16]
    ldp	x30, x21, [sp, #16 * 15] 

    .if \el == EL0
        msr sp_el0, x21
    .endif /* \el == 0 */

    msr elr_el1, x22
    msr spsr_el1, x23

    ldp	x0,  x1,  [sp, #16 * 0]
    ldp	x2,  x3,  [sp, #16 * 1]
    ldp	x4,  x5,  [sp, #16 * 2]
    ldp	x6,  x7,  [sp, #16 * 3]
    ldp	x8,  x9,  [sp, #16 * 4]
    ldp	x10, x11, [sp, #16 * 5]
    ldp	x12, x13, [sp, #16 * 6]
    ldp	x14, x15, [sp, #16 * 7]
    ldp	x16, x17, [sp, #16 * 8]
    ldp	x18, x19, [sp, #16 * 9]
    ldp	x20, x21, [sp, #16 * 10]
    ldp	x22, x23, [sp, #16 * 11]
    ldp	x24, x25, [sp, #16 * 12]
    ldp	x26, x27, [sp, #16 * 13]
    ldp	x28, x29, [sp, #16 * 14]
    ldr	x30, [sp, #16 * 15] 
    add	sp, sp, #S_FRAME_SIZE
    eret
.endm

//defaut handler
exception_handler:
    kernel_entry ELx
    bl  show_invalid_entry_message
invalid_hang:
	b invalid_hang

el0_sync_handler:
    kernel_entry EL0
    // read_irq_source
    mrs	x0, esr_el1
    mrs	x1, elr_el1
    mov x2, sp
    bl sync_router
    bl task_preemption
    kernel_exit EL0

el0_irq_handler:
    kernel_entry EL0
    bl irq_router
    bl task_preemption
.global ret_fork_child
ret_fork_child: // child do not need to restore parent's sp
    kernel_exit EL0

el1_sync_handler:
    kernel_entry EL1
    // read_irq_source
    mrs	x0, esr_el1
    mrs	x1, elr_el1
    mov x2, sp
    bl sync_router
    bl task_preemption
    kernel_exit EL1

el1_irq_handler:
    kernel_entry ELx
    bl irq_router
    kernel_exit ELx

/*
 * Exception vectors.
 */
.align 11// vector table should be aligned to 0x800
.global exception_table
exception_table:
// Current EL with SP0
    ventry exception_handler     // Synchronous
    ventry exception_handler     // IRQ
    ventry exception_handler     // FIQ
    ventry exception_handler     // SError
// Current EL with SPx
    ventry el1_sync_handler      // Synchronous
    ventry el1_irq_handler       // IRQ
    ventry exception_handler     // FIQ
    ventry exception_handler     // SError
// Lower EL using AArch64
    ventry el0_sync_handler      // Synchronous
    ventry el0_irq_handler       // IRQ
    ventry exception_handler     // FIQ
    ventry exception_handler     // SError
// Lower EL using AArch32
    ventry exception_handler     // Synchronous
    ventry exception_handler     // IRQ
    ventry exception_handler     // FIQ
    ventry exception_handler     // SError


.globl enable_irq
enable_irq:
    msr    daifclr, #2 
    ret

.globl disable_irq
disable_irq:
    msr	   daifset, #2
    ret