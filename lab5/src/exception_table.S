.macro context_switch, el
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

.if	\el == 0
mrs	x21, sp_el0
.else
add	x21, sp, #256
.endif /* \el == 0 */

mrs	x22, elr_el1
mrs	x23, spsr_el1

stp	x30, x21, [sp, #16 * 15] 
stp	x22, x23, [sp, #16 * 16]
.endm

.macro	context_switch_back, el
ldp	x22, x23, [sp, #16 * 16]
ldp	x30, x21, [sp, #16 * 15] 

.if	\el == 0
msr	sp_el0, x21
.endif /* \el == 0 */

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
add	sp, sp, #256
eret
.endm

// Simple vector table
.global el2_exception_table
.align 11 // vector table should be aligned to 0x800
el2_exception_table:
    b el_not_implement // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    b el2_exception_handler
    .align 7

    context_switch 2
    bl irq_handler
    context_switch_back 2
    .align 7

    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

el2_exception_handler:
    context_switch 2
    mov     x0, #0
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    bl      _el2_exception_handler
    context_switch_back 2

// Simple vector table
.global el1_exception_table
.align 11 // vector table should be aligned to 0x800
el1_exception_table:
    b el_not_implement // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    b el1_exception_handler
    .align 7
    b el1_irq_handler
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    // from el0
    
    b el1_exception_handler
    .align 7
    b el1_irq_handler
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7

    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7
    b el_not_implement
    .align 7


push_reg_el0:
	sub	sp, sp, #272
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

        mrs	x21, sp_el0
	mrs	x22, elr_el1
	mrs	x23, spsr_el1
    
	stp     x30, x21, [sp, #16 * 15] 
	stp     x22, x23, [sp, #16 * 16]        
		
	b el1_exception_handler

el1_exception_handler:
    context_switch 0
    mov     x5, x0
    mov     x6, x1
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    bl      _el1_exception_handler
    context_switch_back 0
    
el1_irq_handler:
    context_switch 0
    bl      irq_handler
    // bl      _context_switch_msg
    context_switch_back 0

el_not_implement:
    context_switch 0
    bl      _not_implement
    context_switch_back 0

// sync
el0_sync:
	context_switch 0
	mrs	x25, esr_el1				// read the syndrome register
	lsr	x24, x25, #26 // #ESR_ELx_EC_SHIFT		// exception class
	cmp	x24, #0x15 //ESR_ELx_EC_SVC64			// SVC in 64-bit state
	b.eq	el0_svc

sc_nr	.req	x25					// number of system calls
scno	.req	x26					// syscall number
stbl	.req	x27					// syscall table pointer

el0_svc:
	adr	stbl, sys_call_table			// load syscall table pointer
	uxtw	scno, w8				// syscall number in w8
	mov	sc_nr, #4 //__NR_syscalls
	bl	enable_irq
	cmp     scno, sc_nr                     	// check upper syscall limit
	b.hs	ni_sys

	ldr	x16, [stbl, scno, lsl #3]		// address in the syscall table
	blr	x16					// call sys_* routine
	b	ret_from_syscall
ni_sys:

ret_from_syscall:
	bl	disable_irq				
	str	x0, [sp, #0]				// returned x0 #S_X0
	context_switch_back 0

.globl ret_from_fork
ret_from_fork:
	bl	schedule_tail
	cbz	x19, ret_to_user			// not a kernel thread
	mov	x0, x20
	blr	x19
ret_to_user:
	bl disable_irq				
	context_switch_back 0 
