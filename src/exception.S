.section ".text"
/// Call the function provided by parameter `\handler` after saving the exception context.
/// Provide the context as the first parameter to '\handler'.


.macro STORE_CONTEXT, el
    // Make room on the stack for the exception context.
    sub    sp,  sp,  #16 * 17

    // Store all general purpose registers on the stack.
    stp    x0,  x1,  [sp, #16 * 0]
    stp    x2,  x3,  [sp, #16 * 1]
    stp    x4,  x5,  [sp, #16 * 2]
    stp    x6,  x7,  [sp, #16 * 3]
    stp    x8,  x9,  [sp, #16 * 4]
    stp    x10, x11, [sp, #16 * 5]
    stp    x12, x13, [sp, #16 * 6]
    stp    x14, x15, [sp, #16 * 7]
    stp    x16, x17, [sp, #16 * 8]
    stp    x18, x19, [sp, #16 * 9]
    stp    x20, x21, [sp, #16 * 10]
    stp    x22, x23, [sp, #16 * 11]
    stp    x24, x25, [sp, #16 * 12]
    stp    x26, x27, [sp, #16 * 13]
    stp    x28, x29, [sp, #16 * 14]
    //str    x30,      [sp, #16 * 15]
    mov    x0,  sp

    .if	\el == 0
    // store user context
    mrs x1, tpidr_el1
	mrs	x2, sp_el0
    mrs	x3, elr_el1
	mrs	x4, spsr_el1
    mov x5, #2  // IN_OUT_KERNEL
    str x2, [x1, 8 * 13]
    str x3, [x1, 8 * 14]
    str x4, [x1, 8 * 15]
    str x5, [x1, 8 * 16]
    
    mrs	x21, sp_el0
	.else
	add	x21, sp, #16 * 17
	.endif /* \el == 0 */

    // to return to the user mode pc
	mrs	x22, elr_el1
	mrs	x23, spsr_el1

	stp	x30, x21, [sp, #16 * 15] 
	stp	x22, x23, [sp, #16 * 16]
.endm

.macro LOAD_CONTEXT, el
	ldp	x30, x21, [sp, #16 * 15] 
    ldp	x22, x23, [sp, #16 * 16]

	.if	\el == 0
    mrs x1, tpidr_el1
    mov x5, #2  // IN_OUT_KERNEL
    str x5, [x1, 8 * 16]
    ldr x2, [x1, 8 * 13]
    ldr x3, [x1, 8 * 14]
    ldr x4, [x1, 8 * 15]
	msr	sp_el0, x2
    msr	elr_el1, x3
	msr	spsr_el1, x4

	msr	sp_el0, x21
	.endif /* \el == 0 */

	msr	elr_el1, x22			
	msr	spsr_el1, x23

    ldp    x0,  x1,  [sp, #16 * 0] 
    ldp    x2,  x3,  [sp, #16 * 1]
    ldp    x4,  x5,  [sp, #16 * 2]
    ldp    x6,  x7,  [sp, #16 * 3]
    ldp    x8,  x9,  [sp, #16 * 4]
    ldp    x10, x11, [sp, #16 * 5]
    ldp    x12, x13, [sp, #16 * 6]
    ldp    x14, x15, [sp, #16 * 7]
    ldp    x16, x17, [sp, #16 * 8]
    ldp    x18, x19, [sp, #16 * 9]
    ldp    x20, x21, [sp, #16 * 10]
    ldp    x22, x23, [sp, #16 * 11]
    ldp    x24, x25, [sp, #16 * 12]
    ldp    x26, x27, [sp, #16 * 13]
    ldp    x28, x29, [sp, #16 * 14]
    // ldr    x30,      [sp, #16 * 15]  

    add    sp,  sp,  #16 * 17
.endm

.global ret_to_user
ret_to_user:
    bl disable_irq
    LOAD_CONTEXT 0
    // bl debug
    mov x0, 0 // I am child
    // mrs x1, tpidr_el1
    // ldr x2, [x1, 8 * 13]
    // ldr x3, [x1, 8 * 14]
    // ldr x4, [x1, 8 * 15]
	// msr	sp_el0, x2
    // msr	elr_el1, x3
	// msr	spsr_el1, x4
    eret


__el0_exception_routine:
    STORE_CONTEXT 0
    bl      exception_handler
    LOAD_CONTEXT 0
    eret

__el1_exception_routine:
    STORE_CONTEXT 1
    bl      exception_handler
    LOAD_CONTEXT 1
    eret


__el0_interrupt_routine:
    //bl      disable_irq
    STORE_CONTEXT 0
    //bl      debug
    //bl      exception_handler
    bl      interrupt_handler
    LOAD_CONTEXT 0
    //bl      enable_irq
    eret

__el1_interrupt_routine:
    //bl      disable_irq
    STORE_CONTEXT 1
    //bl      debug
    //bl      exception_handler
    bl      interrupt_handler
    LOAD_CONTEXT 1
    //bl      enable_irq
    eret


.align 11
.global _exception_table
_exception_table:
    // Current exception level with SP_EL0.
    .align 7 // synchronous //  0x000
    mov    x0,  0x000
    b exception_not_implement

    .align 7 // IRQ //  0x080
    mov    x0,  0x080
    b exception_not_implement
    
    .align 7 // FIQ //  0x100
    mov    x0,  0x100
    b exception_not_implement

    .align 7 // SError //  0x180
    mov    x0,  0x180
    b exception_not_implement

    // Current exception level with SP_ELx, x > 0.
    .align 7 // synchronous //  0x200
    b __el0_exception_routine

    .align 7 // IRQ //  0x280
    b __el1_interrupt_routine

    .align 7 // FIQ //  0x300
    mov    x0,  0x300
    b exception_not_implement

    .align 7 // SError //  0x380
    mov    x0,  0x380
    b exception_not_implement

    // Lower exception level, AArch64
    .align 7 // synchronous //  0x400
    b __el0_exception_routine
    
    .align 7 // IRQ //  0x480
    b __el0_interrupt_routine

    .align 7 // FIQ //  0x500
    mov    x0,  0x500
    b exception_not_implement

    .align 7 // SError //  0x580
    mov    x0,  0x580
    b exception_not_implement

    // Lower exception level, AArch32
    .align 7 // synchronous //  0x600
    mov    x0,  0x600
    b exception_not_implement

    .align 7 // IRQ //  0x680
    mov    x0,  0x680
    b exception_not_implement

    .align 7 // FIQ //  0x700
    mov    x0,  0x700
    b exception_not_implement

    .align 7 // SError //  0x780
    mov    x0,  0x780
    b exception_not_implement

    .align 7 //  0x800
