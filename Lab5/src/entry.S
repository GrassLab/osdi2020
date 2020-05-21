

/*****************Vector Table********************/

/***** macro for push in register *****/

.macro kernel_entry
sub	sp, sp, #256 //size of all saved register(16*17)
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

// vector table should be aligned to 0x800
 .align 11
.global exception_table
exception_table:

// Current EL with SP0
    // Synchronous 0x000 
    kernel_entry
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // IRQ 0x080
    .align  7
    kernel_entry
    mov     x0, #1
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // FIQ 0x100
    .align  7
    kernel_entry
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // SError 0x180
    .align  7
    kernel_entry
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

// Current EL with SPx
    // Synchrounous 0x200
    .align  7
    kernel_entry
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // IRQ 0x280
    .align  7
    b       push_reg_el1_irq
after_push_el1_irq:
    bl      irq_handler 
    b       pop_reg_el1

    // FIQ 0x300
    .align  7
    kernel_entry
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // SError 0x380
    .align  7
    kernel_entry
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

// Lower EL using AArch64
    // Synchronous 0x400
    .align 7 
    b push_reg_el0
after_push_el0:
    mov x6, x8 //set up argument 
    mrs	x25, esr_el1		// read the syndrome register
    lsr	x24, x25, #26		// exception class
    cmp	x24, #0x15		// SVC in 64-bit state
    b.ne   not_syscall
    bl	el0_svc_handler
    b   ret_from_syscall
  
    // IRQ 0x0480
    .align  7
    b push_reg_el0_irq //so stupid...right?
after_push_el0_irq:
    bl      irq_handler
    bl      signal_handler // Signal handler before you exit kernel
    b pop_reg_el0
    
    // FIQ 0x500
    .align  7
    kernel_entry
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // SError 0x580
    .align  7
    kernel_entry
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

// Lower EL using AArch32
    // Synchronous 0x600
    .align 7
    kernel_entry
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // IRQ 0x680
    .align  7
    kernel_entry
    mov     x0, #1
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // FIQ 0x700
    .align  7
    kernel_entry
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang

    // SError 0x780
    .align  7
    kernel_entry
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    b err_hang
    
    .align 7

not_syscall:	
        cmp x24, #0x24
        b.eq     el0_da
       	
	mov     x0, #4
    	mrs     x1, esr_el1
    	mrs     x2, elr_el1
    	bl      exception_handler
    	b err_hang

ret_from_syscall: 
        bl disable_irq // disable interrupts before pulling preserved data off the stack
	str	x0, [sp, #0] // save x0 on stack for return
    
end:
	bl     disable_irq
	bl     signal_handler // Signal handler before you exit kernel
        b      pop_reg_el0

el0_da:
	bl enable_irq
	mrs x0, far_el1
	mrs x1, esr_el1
	bl page_fault_handler
	cmp x0, 0
	b.eq end

.global err_hang
err_hang:
	b err_hang


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
		
	b after_push_el0

pop_reg_el0:
	ldp	x22, x23, [sp, #16 * 16]
	ldp	x30, x21, [sp, #16 * 15] 
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
	add	sp, sp, #272	
	eret

push_reg_el0_irq:
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
	b after_push_el0_irq

push_reg_el1_irq:
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

	mrs x22,elr_el1
	mrs x23,spsr_el1
	
	stp	x30, x22,[sp, #16 * 15] 
	str     x23,[sp, #16 * 16]
	b after_push_el1_irq

pop_reg_el1:
	ldr	x23, [sp, #16 * 16]
	ldp	x30, x22, [sp, #16 * 15] 

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
	add	sp, sp, #272	
	eret

.global ret_from_fork
ret_from_fork:
	bl schedule_tail //enable preeption
	cbz	x19, ret_to_user			// not a kernel thread
	blr	x19 //after kernel thread finish, go to ret_to_user
ret_to_user:
	bl disable_irq
	b pop_reg_el0
