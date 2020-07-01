#include "sysregs.h"
#include "mm.h"

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
    /*** Now return from el2 to el1 ***/  
    //disable MMU
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0	
 
    // set HCR_EL2 RW bit to 1 
    // since The execution state of EL1 is AArch64
    mrs x0, hcr_el2
    orr x0, x0, #(1<<31) // RW=1 EL1 Execution state is AArch64.
    msr hcr_el2, x0
    
    // set up exception handlers
    ldr     x2, =el2_exception_table
    msr     vbar_el2, x2

    ldr     x2, =el1_exception_table
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
    adr	x0, _bss_begin
    adr	x1, _bss_end
    sub	x1, x1, x0
    bl 	memzero

    bl          page_table_create 

    mov	x0, #VA_START			
    add	sp, x0, #LOW_MEMORY

    // setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1,x0
  
    // setup MAIR
    ldr	x0, = MAIR_CONFIG_DEFAULT
    msr	mair_el1, x0

    // indirect branch
    // load before turn on MMU
    ldr x0 ,=kernel_main  
    
    mrs x2, sctlr_el1
    orr x2 , x2, 1 
    msr sctlr_el1, x2 // enable MMU, cache remains disabled
main:
    br      x0
    // for failsafe, halt this core too
    b       proc_hang

/*******************************************************************/

page_table_create:
    mov x29,x30
    
    // use physical address of page table
    ldr x0, =pg_dir
    lsl x0,x0,#16
    lsr x0,x0,#16
 
    add  x1, x0, #PAGE_SIZE // PUD's page frame
    add  x2, x1, #PAGE_SIZE  // 1st PMD's page frame
    add  x14, x2, #PAGE_SIZE // 2nd PMD's page frame
    add  x15, x14, #PAGE_SIZE // 1st PTE's page frame
                              // totally 1024 page (2G)

    // step 1. setting PGD 
    ldr x3, = PD_TABLE // means it is a table descriptor
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]
    
    // step 2. setting PUD
    ldr x3, = PD_TABLE
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
             
    ldr x3, = PD_TABLE 
    orr x3, x14, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD


    // step 3. setting PMD
    mov x4, x15 //x4 for PTE's page table
    mov x5, xzr
    mov x6, #1024
3:
    ldr x3, = PD_TABLE
    orr x3, x4, x3
    str x3, [x2, x5, lsl #3] 
    add x5,x5,#1    
    add x4, x4, #PAGE_SIZE //next PTE page
    cmp x5,x6 
    b.ls 3b

    // step4. setting PTE
    mov x4, xzr //x4 for physical memory
    mov x5, xzr
    mov x6, #0x41000000 
1:     
    ldr x3, = MMU_DEVICE_FLAGS
    orr x3, x4, x3
    str x3, [x15, x5, lsl #3]
    add x5, x5, #1
    add x4, x4, #PAGE_SIZE 
    cmp x4, x6 
    b.ls 1b
 
    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
 
    mov x30,x29
    ret
