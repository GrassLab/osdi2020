/*
 * Memory management setup
 */
#include "mmu.h"
#include "mm.h"
.global page_table_create
page_table_create:
    //mov x29,x30
    
    // use physical address of page table
    ldr x0, =page_dir
    lsl x0,x0,#16
    lsr x0,x0,#16
 
    add  x1, x0, #PAGE_SIZE // PUD's page frame
    add  x2, x1, #PAGE_SIZE  // 1st PMD's page frame
    add  x14, x2, #PAGE_SIZE // 2nd PMD's page frame
    add  x15, x14, #PAGE_SIZE // 1st PTE's page frame
                              // totally 1024 page (2G)

    // step 1. setting PGD 
    ldr x3, =PD_TABLE // means it is a table descriptor
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]
    
    // step 2. setting PUD
    ldr x3, =PD_TABLE
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
             
    ldr x3, =PD_TABLE 
    orr x3, x14, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD


    // step 3. setting PMD
    mov x4, x15 //x4 for PTE's page table
    mov x5, xzr
    mov x6, #1024
3:
    ldr x3, =PD_TABLE
    orr x3, x4, x3
    str x3, [x2, x5, lsl #3] 
    add x5,x5,#1    
    add x4, x4, #PAGE_SIZE //next PTE page
    cmp x5,x6 
    b.ls 3b

    // step4. setting PTE
    mov x4, xzr //x4 for physical memory
    mov x5, xzr
    mov x6, 0x40000000
1:     
    ldr x3, =MMU_DEVICE_FLAGS
    orr x3, x4, x3
    str x3, [x15, x5, lsl #3]
    add x5, x5, #1
    add x4, x4, #PAGE_SIZE 
    cmp x4, x6 
    b.ls 1b
 
    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
 
    //mov x30,x29
    ret


.globl set_pgd
set_pgd:
	msr	ttbr0_el1, x0
	tlbi vmalle1is // invalidate all TLB entries
  	dsb ish        // ensure completion of TLB invalidatation
	isb            // clear pipeline
	ret