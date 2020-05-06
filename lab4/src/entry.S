#include "entry.h"

	.macro handle_invalid_entry type
	kernel_entry 0
	mov	x0, #\type
	mrs	x1, esr_el1
	mrs	x2, elr_el1
	bl	show_invalid_entry_message
	b err_hang
	kernel_exit 0
	.endm

	.macro	ventry	label
	.align	7
	b	\label
	.endm

	.macro	kernel_entry, el
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
	
	.if \el == 0
	mrs x21, sp_el0
	.else
	add x21, sp, #272
	.endif /* \el == 0 */

	mrs x22, elr_el1
	mrs x23, spsr_el1

	stp x30, x21, [sp, #16 * 15]
	stp x22, x23, [sp, #16 * 16]
	.endm



	.macro	kernel_exit, el
	ldp x22, x23, [sp, #16 * 16]
	ldp x30, x21, [sp, #16 * 15]

	.if \el == 0
	msr sp_el0, x21
	.endif /* \el == 0 */

	msr elr_el1, x22
	msr spsr_el1, x23

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
	add	sp, sp, #272	
	eret
	.endm

//***************************************************************************
//	EXCEPTION VECTOR BEGIN
//***************************************************************************

/*
 * Exception vectors.
 */
.align	11
.globl vectors 
vectors:
	ventry	sync_invalid_el1t			// Synchronous EL1t
	ventry	irq_invalid_el1t			// IRQ EL1t
	ventry	fiq_invalid_el1t			// FIQ EL1t
	ventry	error_invalid_el1t			// Error EL1t

	ventry	sync_invalid_el1h			// Synchronous EL1h
	ventry	irq_invalid_el1h			// IRQ EL1h
	ventry	fiq_invalid_el1h			// FIQ EL1h
	ventry	error_invalid_el1h			// Error EL1h

	ventry	sync_invalid_el0_64			// Synchronous 64-bit EL0
	ventry	irq_invalid_el0_64			// IRQ 64-bit EL0
	ventry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	ventry	error_invalid_el0_64		// Error 64-bit EL0

	ventry	sync_invalid_el0_32			// Synchronous 32-bit EL0
	ventry	irq_invalid_el0_32			// IRQ 32-bit EL0
	ventry	fiq_invalid_el0_32			// FIQ 32-bit EL0
	ventry	error_invalid_el0_32		// Error 32-bit EL0

/*
 *  EL1t: same level using SP_EL0(0x000-0x1FF)
 */
sync_invalid_el1t:
	handle_invalid_entry  SYNC_INVALID_EL1t		// 0

irq_invalid_el1t:
	handle_invalid_entry  IRQ_INVALID_EL1t		// 1

fiq_invalid_el1t:
	handle_invalid_entry  FIQ_INVALID_EL1t		// 2

error_invalid_el1t:
	handle_invalid_entry  ERROR_INVALID_EL1t	// 3

/*
 *	EL1h: same level using SP_ELx(0x200-0x3FF)
 */
sync_invalid_el1h:
	bl el1_sync_handler	//	svc handler

irq_invalid_el1h:
	bl el1_irq_handler	// timer handler

fiq_invalid_el1h:
	handle_invalid_entry  FIQ_INVALID_EL1h		// 6

error_invalid_el1h:
	handle_invalid_entry  ERROR_INVALID_EL1h	// 7

/*
 * EL0_64: lower level with AArch64(0x400-0x5FF)
 */
sync_invalid_el0_64:
	bl el0_sync_handler	//	svc handler

irq_invalid_el0_64:
	bl el0_irq_handler	// timer handler

fiq_invalid_el0_64:
	handle_invalid_entry  FIQ_INVALID_EL0_64	// 10

error_invalid_el0_64:
	handle_invalid_entry  ERROR_INVALID_EL0_64	// 11

/*
 * EL0_32: lower level with AArch32(0x600-0x7FF)
 */
sync_invalid_el0_32:
	handle_invalid_entry  SYNC_INVALID_EL0_32	// 12

irq_invalid_el0_32:
	handle_invalid_entry  IRQ_INVALID_EL0_32	// 13

fiq_invalid_el0_32:
	handle_invalid_entry  FIQ_INVALID_EL0_32	// 14

error_invalid_el0_32:
	handle_invalid_entry  ERROR_INVALID_EL0_32	// 15


//***************************************************************************
//	EXCEPTION VECTOR END
//***************************************************************************

el1_sync_handler:
	kernel_entry 1	// store general registers
	bl sync_handler	//	svc handler
	kernel_exit	1	// load general registers

el1_irq_handler:
	kernel_entry 1	// store general registers
	bl irq_handler	//	svc handler
	kernel_exit	1	// load general registers

el0_sync_handler:
	kernel_entry 0	// store general registers
	bl sync_handler	//	svc handler
	kernel_exit	0	// load general registers

el0_irq_handler:
	kernel_entry 0	// store general registers
	bl irq_handler	//	svc handler
	kernel_exit	0	// load general registers

.globl err_hang
err_hang: b err_hang


.global ret_from_fork
ret_from_fork:
    cbz   x19, ret_to_user
    blr   x19
ret_to_user:
    bl disable_irq
    kernel_exit 0

