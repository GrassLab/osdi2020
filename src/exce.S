
.macro  kernel_entry
    sub	sp, sp, #256
    stp	x0, x1, [sp, #16 * 0]
    stp	x2, x3, [sp, #16 * 1]
    stp	x4, x5, [sp, #16 * 2]
    stp	x6, x7, [sp, #16 * 3]
    stp	x8, x9, [sp, #16 * 4]
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
    str	x30, [sp, #16 * 15]
.endm

.macro  kernel_exit
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
    ldp	x28, x29, [sp, #16 * 14]
    ldr	x30, [sp, #16 * 15] 
    add	sp, sp, #256		
    eret
.endm

//defaut handler
exception_handler:
    kernel_entry
    bl  show_invalid_entry_message
invalid_hang:
	b invalid_hang

sync_handler:
    kernel_entry
    // read_irq_source
    mrs	x0, esr_el1
    mrs	x1, elr_el1
    mrs	x2, spsr_el1
    mrs	x3, far_el1
    bl sync_router
    kernel_exit

irq_handler:
    kernel_entry
    bl irq_router
    kernel_exit

/*
 * Exception vectors.
 */
.align 11// vector table should be aligned to 0x800
.global exception_table
exception_table:
// Current EL with SP0
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError
// Current EL with SPx
    .align 7
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError
// Lower EL using AArch64
    .align 7
    b sync_handler          // Synchronous
    .align 7
    b irq_handler           // IRQ
    .align 7 
    b exception_handler     // FIQ
    .align 7 
    b exception_handler     // SError
// Lower EL using AArch32
    .align 7
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError


.globl enable_irq
enable_irq:
    msr    daifclr, #2 
    ret

.globl disable_irq
disable_irq:
    msr	   daifset, #2
    ret