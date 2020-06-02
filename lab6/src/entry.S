#include "entry.h"
#include "sys.h"

    .macro handle_invalid_entry el, type 
	kernel_entry \el
	mov	x0, #\type
	mrs	x1, esr_el1
	mrs	x2, elr_el1
	bl	show_invalid_entry_message
    kernel_exit \el
	.endm

    .macro handel_sync_el0_entry 
    kernel_entry 0
    bl  sync_handler
    str x0, [sp, #0]             // returned x0
    kernel_exit 0
    eret
    .endm
    
    .macro handel_sync_el1_entry
    kernel_entry 1
    bl  sync_handler
    kernel_exit 1
    eret
    .endm

    .macro handle_irq_el0_entry
	kernel_entry 0
	bl	irq_handler
	kernel_exit 0
    .endm

    .macro handle_irq_el1_entry
	kernel_entry 1
	bl	irq_handler
	kernel_exit 1
    .endm

	.macro	ventry	label
	.align	7
	b	\label
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

/*why*/
    
    .if	\el == 0
	mrs	x21, sp_el0
	.else
	add	x21, sp, #S_FRAME_SIZE
	.endif 

	mrs	x22, elr_el1
	mrs	x23, spsr_el1

	stp	x30, x21, [sp, #16 * 15] 
	stp	x22, x23, [sp, #16 * 16]	
    
	.endm

	.macro	kernel_exit, el
	ldp	x22, x23, [sp, #16 * 16]
	ldp	x30, x21, [sp, #16 * 15] 

/*why*/

    .if \el == 0
    msr	sp_el0, x21
    .endif

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


/*
 * Exception vectors.
 */
.align	11
.globl vectors 
vectors:
	ventry	sync_invalid_elxt			// Synchronous EL1t
	ventry	irq_invalid_elxt			// IRQ EL1t
	ventry	fiq_invalid_elxt			// FIQ EL1t
	ventry	error_invalid_elxt			// Error EL1t

	ventry	sync_invalid_elxh			// Synchronous EL1h
	ventry	irq_invalid_elxh			// IRQ EL1h
	ventry	fiq_invalid_elxh			// FIQ EL1h
	ventry	error_invalid_elxh			// Error EL1h

	ventry	sync_invalid_elxm1_64			// Synchronous 64-bit EL0
	ventry	irq_invalid_elxm1_64			// IRQ 64-bit EL0
	ventry	fiq_invalid_elxm1_64			// FIQ 64-bit EL0
	ventry	error_invalid_elxm1_64			// Error 64-bit EL0

	ventry	sync_invalid_elxm1_32			// Synchronous 32-bit EL0
	ventry	irq_invalid_elxm1_32			// IRQ 32-bit EL0
	ventry	fiq_invalid_elxm1_32			// FIQ 32-bit EL0
	ventry	error_invalid_elxm1_32			// Error 32-bit EL0

sync_invalid_elxt:
	handle_invalid_entry  1,SYNC_INVALID_ELxt

irq_invalid_elxt:
	handle_invalid_entry  1,IRQ_INVALID_ELxt

fiq_invalid_elxt:
	handle_invalid_entry  1,FIQ_INVALID_ELxt

error_invalid_elxt:
	handle_invalid_entry  1,ERROR_INVALID_ELxt

sync_invalid_elxh:
    handel_sync_el1_entry

irq_invalid_elxh:
    handle_irq_el1_entry

fiq_invalid_elxh:
	handle_invalid_entry  1,FIQ_INVALID_ELxh

error_invalid_elxh:
	handle_invalid_entry  1,ERROR_INVALID_ELxh

sync_invalid_elxm1_64:
    handel_sync_el0_entry

irq_invalid_elxm1_64:
	handle_irq_el0_entry

fiq_invalid_elxm1_64:
	handle_invalid_entry  0,FIQ_INVALID_ELxm1_64

error_invalid_elxm1_64:
	handle_invalid_entry  0,ERROR_INVALID_ELxm1_64

sync_invalid_elxm1_32:
	handle_invalid_entry  0,SYNC_INVALID_ELxm1_32

irq_invalid_elxm1_32:
	handle_invalid_entry  0,IRQ_INVALID_ELxm1_32

fiq_invalid_elxm1_32:
	handle_invalid_entry  0,FIQ_INVALID_ELxm1_32

error_invalid_elxm1_32:
	handle_invalid_entry  0,ERROR_INVALID_ELxm1_32
    
.globl ret_from_fork
ret_from_fork:
    bl  preempt_enable
    cbz x19, ret_to_user            // not a kernel thread
    mov x0, x20
    blr x19         //should never return
ret_to_user:
    bl disable_irq
    kernel_exit 0

.globl err_hang
err_hang: b err_hang
