#include "include/sys.h"

.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x0, mpidr_el1  // put mpidr_el1(process id) in x0
    and     x0, x0, #0xFF // Check processor id(x0= x0 and 0xFF)
    cbz     x0, master // Hang for all non-primary CPU
    
    //(if x0 == 0 jump to master)
    b       proc_hang    //Else it will become a for loop


proc_hang:
    b proc_hang

master:
    // set stack before our code
    ldr     x1, =_start
    

    /*** Now return from el2 to el1 ***/ 
    mov x0,0xF0000 //Hey, you gonna set a different memory for stack
    msr sp_el1,x0  //set stack pointer
    
    // set HCR_EL2 RW bit to 1 
    // since The execution state of EL1 is AArch64
    mrs x0, hcr_el2
    orr x0, x0, #(1<<31) // RW=1 EL1 Execution state is AArch64.
    msr hcr_el2, x0
    
    // set up exception handlers
    ldr     x2, = exception_table
    msr     vbar_el1, x2

    // change execution level
    mov x2, #0b0101 //selcet stack pointer for EL1h
    msr spsr_el2,x2
    
    adr x2,el1_entry
    msr elr_el2,x2
    eret

    /*** Then, return from el1 to el0 ***/

el1_entry:
    
    /*** Now return from el2 to el1 ***/
    msr sp_el0,x1  //set stack pointer

    // set up exception handlers
    ldr     x2, = exception_table
    msr     vbar_el1, x2
 
    // Determine the EL0 Execution state.
    mov x0, #0b00000 // DAIF=0000 M[4:0]=00000 EL0t.     
    msr spsr_el1, x0
   
    
    adr x0, el0_entry 
    msr elr_el1, x0 // EL0 code.
    eret

el0_entry:

    // clear bss
    ldr     x1, =_bss_begin
    ldr     w2, =__bss_size
clean:
    cbz     w2, main
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clean

    // jump to C code, should not return
main:  
    bl      kernel_main
    // for failsafe, halt this core too
    b       proc_hang

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
    bl push_reg 
    str	x30, [sp, #16 * 15] //have to store x30 in vector table
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    bl      exception_handler
    bl pop_reg
    ldr     x30, [sp,#16 * 15] //have to load x30 in vector table
    add  sp, sp, #256            // and add stack pointer back
    
    eret

    // IRQ 0x280
    .align  7
    bl push_reg
    str	x30, [sp, #16 * 15] //have to store x30 in vector table
    bl      irq_handler
    bl pop_reg
    ldr     x30, [sp,#16 * 15] //have to load x30 in vector table
    add  sp, sp, #256            // and add stack pointer back
    eret

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

after_push:    
    //bl print_this
    mrs	x25, esr_el1				// read the syndrome register
    lsr	x24, x25, #26		// exception class
    cmp	x24, #0x15			// SVC in 64-bit state
    b.eq	el0_svc
    b          ni_sys
  

    // IRQ 0x0480
    .align  7  
    b push_reg_el0_irq //so stupid...right?

after_push_irq:        
    bl      irq_handler
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

//x25: number of system calls
//x26: syscall number
//x27: syscall table pointer

el0_svc:
	adr	x27, sys_call_table	// load syscall table pointer
	uxtw	x26, w8			// syscall number in w8
        mov	x25, #__NR_syscalls
	bl	enable_irq
	cmp     x26, x25                // check upper syscall limit	 	
	b.hs	ni_sys

	ldr	x16, [x27, x26, lsl #3] // address in the syscall table
	blr	x16			// call sys_* routine
	b	ret_from_syscall
ni_sys:
	mov     x0, #4
    	mrs     x1, esr_el1
    	mrs     x2, elr_el1
    	bl      exception_handler
    	b err_hang

ret_from_syscall: 
	bl	disable_irq				
	str	x0, [sp, #0] // returned x0
        b pop_reg_el0


.global err_hang
err_hang:
	b err_hang


.global push_reg
push_reg:
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
	

.global	pop_reg
pop_reg:
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
	ret



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
	b after_push

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
	b after_push_irq
