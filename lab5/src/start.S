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
    
    //setting TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0
    
    //setting MAIR
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, = BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    ldr x2, boot_rest // indirect branch to the virtual address
    br x2

boot_rest:

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


