.macro	kernel_entry
sub	sp, sp, #280
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

// save user context
mrs	x21, sp_el0
mrs	x22, elr_el1
mrs	x23, spsr_el1
str x21, [sp, 8 * 16]
str x22, [sp, 8 * 17]
str x23, [sp, 8 * 18]

mov x0,  sp
.endm

.macro	kernel_exit

// restore user context
ldr x21, [sp, 8 * 16]
ldr x22, [sp, 8 * 17]
ldr x23, [sp, 8 * 18]
msr	sp_el0, x21
msr	elr_el1, x22
msr	spsr_el1, x23

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
add	sp, sp, #280		
eret
.endm

.macro	ventry	label
.align	7
b	\label
.endm


.align 11      // important, code has to be properly aligned
.global exception_table
exception_table:
    ventry el1_exception
    ventry el1_exception
    ventry el1_exception
    ventry el1_exception
    // synchronous
    ventry el1_exception
    // IRQ
    ventry el1_irq
    // FIQ
    ventry el1_exception
    // SError
    ventry el1_exception

    ventry el1_exception
    ventry el1_irq
    ventry el1_exception
    ventry el1_exception

el1_exception:
    kernel_entry
    bl exception_handler
    kernel_exit

el1_irq:
    kernel_entry
    bl irq_handler
    kernel_exit