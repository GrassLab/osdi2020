
#include "sys.h"

#define S_FRAME_SIZE			272 		// size of all saved registers 
#define S_X0				0		// offset of x0 register in saved stack frame
#define ESR_ELx_EC_SHIFT		26
#define ESR_ELx_EC_SVC64		0x15

// define exception type
#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    	8 
#define IRQ_INVALID_EL0_64	    	9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64		11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32		15 

#define SYNC_ERROR			16 
#define SYSCALL_ERROR			17 

.section ".text"

.macro handle_invalid_entry el, type
kernel_entry \el
mov	x0, #\type
mrs	x1, esr_el1
mrs	x2, elr_el1
bl	show_invalid_entry_message
b	err_hang
.endm

.macro	kernel_entry, el
sub	sp, sp, #S_FRAME_SIZE
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
add	x21, sp, #S_FRAME_SIZE
.endif /* \el == 0 */

mrs	x22, elr_el1
mrs	x23, spsr_el1

stp	x30, x21, [sp, #16 * 15] 
stp	x22, x23, [sp, #16 * 16]
.endm

.macro	kernel_exit, el
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
add	sp, sp, #S_FRAME_SIZE		
eret
.endm

.macro	ventry	label
.align	7
b	   \label
.endm

.align 11
.global exception_table
exception_table:
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry el0_exception
    ventry el0_irq_handler
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

el0_exception:
    //kernel_entry
	//mov     x0, #0
    //mrs     x1, esr_el1
    //mrs     x2, elr_el1
    //bl exception_handler
    //kernel_exit
	kernel_entry 0
    mrs    x25, esr_el1                // read the syndrome register
    lsr    x24, x25, #ESR_ELx_EC_SHIFT // exception class
    cmp    x24, #ESR_ELx_EC_SVC64      // SVC in 64-bit state
    b.eq   el0_svc
    handle_invalid_entry 0, SYNC_ERROR

sc_nr	.req	x25					// number of system calls
scno	.req	x26					// syscall number
stbl	.req	x27					// syscall table pointer

el0_svc:
	adr	stbl, sys_call_table			// load syscall table pointer
	uxtw	scno, w8				// syscall number in w8
	mov	sc_nr, #__NR_syscalls
	bl	enable_irq
	cmp     scno, sc_nr                     	// check upper syscall limit
	b.hs	ni_sys

	ldr	x16, [stbl, scno, lsl #3]		// address in the syscall table
	blr	x16					// call sys_* routine
	b	ret_from_syscall

ret_from_syscall:
	bl	disable_irq				
	str	x0, [sp, #S_X0]				// returned x0
	kernel_exit 0

el0_irq_handler:
	kernel_entry 0
	bl irq_router
	kernel_exit 0

el1_irq_handler:
	kernel_entry 1
	bl irq_router
	kernel_exit 1

.globl ret_from_fork
ret_from_fork:
	//bl	schedule_tail
	mov	x0, x20
	blr	x19 		//should never return

ni_sys:
	handle_invalid_entry 0, SYSCALL_ERROR

.globl err_hang
err_hang: b err_hang