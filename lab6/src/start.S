#include "mm.h"
.section ".text.boot"
#define USER_STACK 0x1000

.globl _start
_start:
    //read cpu id
	mrs x0, mpidr_el1   //Move to ARM register from system coprocessor register
	and x0, x0, 3
	cbz x0, 2f

// let core with cpu id != 0 enter busy loop
1:
	wfe
	b _start

//cpu id == 0
2:
	// set stack
    mov x1, #(2 * SECTION_SIZE) 
//	ldr x1, =_start

	// get CurrentEL
    mrs x0, CurrentEL
    and x0, x0, #12 // clear reserved bits

    // running at EL3?
    cmp x0, #12
    bne 5f
    // should never be executed, just for completeness
    mov x2, #0x5b1
    msr scr_el3, x2
    mov x2, #0x3c9
    msr spsr_el3, x2
    adr x2, 5f
    msr elr_el3, x2
    eret
    
	// running at EL2? 
5:
    mrs x0, CurrentEL
    and x0, x0, #12 // clear reserved bits
    cmp x0, #8
    bne 1b
    bl  irq_vector2_init
    bl  exception_rount_el2
    bl  from_el2_to_el1

from_el2_to_el1:
    mov x1, #(2 * SECTION_SIZE) 
//    ldr x1, =_start
    msr     sp_el1, x1 // init sp for el1 option 1
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)  // EL1 use aarch64
    msr     hcr_el2, x0
    // change execution level to EL1
    mov     x2, #0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr     spsr_el2, x2
    adr     x2, 5f  
    msr     elr_el2, x2  // init sp for el1 option 2
    eret // return to EL1

5: 
//	ldr x1, =_start
//  mov sp, x1
//    mov     sp, #(2 * SECTION_SIZE) 

    // clear bss
	ldr x1, =__bss_start
	ldr w2, =__bss_size
3:	cbz w2, 6f
	str xzr, [x1], 8 //fill zero 
	sub w2, w2, 1
    cbnz w2, 3b
    
   //running at el1 ?
6: 
    mrs x0, CurrentEL
    and x0, x0, #12 // clear reserved bits
    cmp     x0, #4
    bne     1b
    bl      irq_vector1_init
//    bl      from_el1_to_el0
    b   4f

from_el1_to_el0:  	
    mov x0, USER_STACK
	msr sp_el0, x0
	mov x0, 0 // EL0 with interrupt enabled
  	msr spsr_el1, x0
  	adr x0, 4f // return to shell run in EL0
  	msr elr_el1, x0
  	eret // return to EL0

// jump to main function
4:     
//    mov sp, USER_STACK	
    mov sp, #(2 * SECTION_SIZE) 
	bl main
	b 1b


