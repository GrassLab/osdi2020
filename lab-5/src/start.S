#include "register.h"
#include "memory.h"

.section ".text.boot"
.globl _start
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
    /*** Now return from el2 to el1 ***/  
    //disable MMU
    // ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    // msr	sctlr_el1, x0	

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

el1_entry:
    mov x0,#LOW_MEMORY 
    mov sp, x0
    
    //disable exception of access SIMD register in EL1(for printf)
    mrs x0, CPACR_EL1
    orr x0, x0, #(3 << 20)
    msr CPACR_EL1, x0

    // clear bss
    adr	x0, __bss_start
    adr	x1, __bss_end
    sub	x1, x1, x0
    bl 	memzero

    bl  page_table_create 

    mov	x0, #VA_START
    mov x1,#LOW_MEMORY			
    add	sp, x0, x1

    // setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1,x0

    // setup MAIR
    ldr	x0, = MAIR_CONFIG_DEFAULT
    msr	mair_el1, x0

    mrs x2, sctlr_el1
    orr x2 , x2, 1 
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    // indirect branch
    ldr x0 ,=main  
    br      x0
    b       proc_hang

/*******************************************************************/

page_table_create:
    mov x29,x30

  	mov     x0, 0     			// PGD addr
    mov     x1, 0x1000 			// PUD addr
    mov     x2, 0x2000 			// PMD addr
    // create page table
    ldr     x7, =BOOT_PGD_ATTR
    orr     x7, x1, x7          // combine the physical address of next level page with attribute.
    str     x7, [x0]            // set 1st PGD entry(pointer to PUD)

    ldr     x7, =BOOT_PUD_ATTR
    orr     x7, x2, x7          // combine the physical address of next level page with attribute(PMD).
    str     x7, [x1]            // setup 1st PUD entry(pointer to PMD)

	ldr     x7, =PT_PMD_DEV_ATTR
    orr     x7, x2, x7          // combine the physical address of next level page with attribute(PMD).
    str     x7, [x1, #8]            // setup 2nd PUD entry(pointer to PMD) for core timer interrupt CORE0_TIMER_IRQ_CTRL

    ldr     x7, =PT_PMD_NOR_ATTR
    mov     x6, xzr               // PMD idx conter 0,1,2,3...
nor_pmd:                          // 0x000 00000 to 0x3ef fffff(0x2000 to 0x3f78)
    mov     x5, x6, lsl #21       // x5 = (x6 << 21) ,x5 = 2MB, 4MB,...
    orr     x5, x5, x7            // combine block address and attribute to a descriptor
    str     x5, [x2, x6, lsl #3]  // set PMD table entry(${x6}th entry)
    add     x6, x6, #1			  // count++
    cmp     x6, #(0x3f000000 >> 21)	  // compare if arrive margin
    blo     nor_pmd

    ldr     x7, =PT_PMD_DEV_ATTR  // set gpio MMBASE 0x3F to device memory
dev_pmd:                          // 0x3F0 00000 to 0x3FF FFFFF(0x3f80 to 0x3ff8)
    mov     x5, x6, lsl #21       // x5 = (x6 << 21) ,x5 = 2MB, 4MB,...
    orr     x5, x5, x7            // combine block address and attribute to a descriptor
    str     x5, [x2, x6, lsl #3]  // set PMD table entry(${x6}th entry)
    add     x6, x6, #1  		  // count++
    cmp     x6, #(0x40000000 >> 21)	  // compare if arrive margin
    blo     dev_pmd

    msr     ttbr0_el1, x0         // load PGD to the buttom translation based register.
    msr     ttbr1_el1, x0         // also load PGD to the upper translation based register.

    mov x30,x29
    ret

//    mov x0, 0 // PGD's page frame at 0x0
//    mov x1, 0x1000 // PUD's page frame at 0x1000
//
//    ldr x2, = BOOT_PGD_ATTR
//    orr x2, x1, x2 // combine the physical address of next level page with attribute.
//    str x2, [x0]
//
//    ldr x2, = BOOT_PUD_ATTR
//    mov x3, 0x00000000
//    orr x3, x2, x3
//    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
//    mov x3, 0x40000000
//    orr x3, x2, x3
//    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD
//
//    msr ttbr0_el1, x0
//    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
//    mrs x2, sctlr_el1
//    orr x2 , x2, 1
//    msr sctlr_el1, x2
//    ret


//     mov     x0, 0x0
//     mov     x1, 0x1000 // 4096
//     mov     x2, 0x2000
//     // create page table
//     ldr     x7, =BOOT_PGD_ATTR
//     orr     x7, x1, x7          // combine the physical address of next level page with attribute.
//     str     x7, [x0]            // setup first PGD entry(pointer to PUD)
// 
//     ldr     x7, =BOOT_PUD_ATTR
//     orr     x7, x2, x7          // combine the physical address of next level page with attribute.
//     str     x7, [x1]            // setup first PUD entry(pointer to PMD)
// 
//     ldr     x7, =BOOT_PMD_NORMAL_ATTR
//     mov     x6, xzr             // PMD idx conter
// nor_pmd:                          //0x000 00000 to 0x3ef fffff(0x2000 to 0x3f78)
//     mov     x5, x6, lsl #21       // block basis
//     orr     x5, x5, x7            // combine block address and attribute to a descriptor
//     str     x5, [x2, x6, lsl #3]
//     add     x6, x6, #1
//     cmp     x6, #0x3F0
//     blo     nor_pmd
// 
//     ldr     x7, =BOOT_PMD_DEVICE_ATTR
// dev_pmd:                          //0x3F0 00000 to 0x3FF FFFFF(0x3f80 to 0x3ff8)
//     mov     x5, x6, lsl #21       // block basis
//     orr     x5, x5, x7            // combine block address and attribute to a descriptor
//     str     x5, [x2, x6, lsl #3]
//     add     x6, x6, #1
//     cmp     x6, #0x400 // blocks
//     blo     dev_pmd
//     msr     ttbr0_el1, x0         // load PGD to the buttom translation based register.
//     msr     ttbr1_el1, x0         // also load PGD to the upper translation based register.
// 
//     ldr     x2, =main_entry       // indirect branch to the virtual address
// 
//     isb
//     mrs     x0, sctlr_el1
//     orr     x0 , x0, SCTLR_MMU_ENABLED
//     msr     sctlr_el1, x0         // enable MMU, cache remains disabled
//     isb
// 
//     br      x2