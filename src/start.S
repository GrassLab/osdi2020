.section ".text.boot"

#include "../include/sysregs.h"
#include "../include/mm.h"
#include "../include/mmu.h"

.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    // cpu id == 0
    cbnz     x1, loop


_setup_el1:
    //disable MMU
    ldr	    x0, =SCTLR_VALUE_MMU_DISABLED
    msr	    sctlr_el1, x0	
    
    // check if run in EL2
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits //lsr x0, x0, #2

    // set up exception handlers
    ldr     x0, =_exception_table
    msr     vbar_el1, x0

    // set stack before our code
    ldr     x1, =_start
    msr     sp_el1, x1

    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    
    // change execution level to EL1
    mov     x0, 0x5//#0x3c4
    msr     spsr_el2, x0
    adr     x0, _run_main  // from el2 return to el1
    msr     elr_el2, x0
    eret


// #define USER_STACK 0x1000

// _setup_el0:
//     // disable exception of access SIMD register in EL1
//     mrs     x0, cpacr_el1
//     orr     x0, x0, #(3 << 20)
//     msr     cpacr_el1, x0

//     // setup EL0 stack
//     mov     x1, USER_STACK
//     msr     sp_el0, x1
//     // switch to EL0
//     // spsr_el1 clear daif and switch to EL0
//     mov     x1, 0x0
//     msr     spsr_el1, x1
//     adr     x1, _run_main  // from el1 return to el0
//     msr     elr_el1, x1
//     eret


// #define MAIR_DEVICE_nGnRnE 0b00000000
// #define MAIR_NORMAL_NOCACHE 0b01000100
// #define MAIR_IDX_DEVICE_nGnRnE 0
// #define MAIR_IDX_NORMAL_NOCACHE 1

// #define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
// #define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
// #define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

// #define PD_TABLE 0b11
// #define PD_BLOCK 0b01
// #define PD_ACCESS (1 << 10)

// #define MAIR_CONFIG_DEFAULT ( \
//         (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
//         (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
//     )

_run_main:
    // set up stack
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

    // bl __create_page_tables // 3-level
    bl page_table_create // 4-level

    // setup kernel stack
    mov	x0, #VA_START			
	add	sp, x0, #LOW_MEMORY

    // setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    // setup MAIR
    ldr x0, = MAIR_CONFIG_DEFAULT
    msr mair_el1, x0



    // mov x0, 0 // PGD's page frame at 0x0
    // mov x1, 0x1000 // PUD's page frame at 0x1000

    // ldr x2, = BOOT_PGD_ATTR
    // orr x2, x1, x2 // combine the physical address of next level page with attribute.
    // str x2, [x0]

    // ldr x2, = BOOT_PUD_ATTR
    // mov x3, 0x00000000
    // orr x3, x2, x3
    // str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    // mov x3, 0x40000000
    // orr x3, x2, x3
    // str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    // msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    // mrs x2, sctlr_el1
    // orr x2 , x2, 1
    // msr sctlr_el1, x2 // enable MMU, cache remains disabled


    // mov	x0, x0, #LOW_MEMORY

	// ldr	x0, =(TCR_VALUE)		
	// msr	tcr_el1, x0

	// ldr	x0, =(MAIR_VALUE)
	// msr	mair_el1, x0
    

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    ldr	x0, =main

    // jump to C code, should not return
    // bl      main
    
    br x0
loop:  
    // for failsafe, halt this core too
    // cpu id > 0, stop
    wfe             // wait for event 
    b       loop


page_table_create:
    mov x29,x30
    
    // use physical address of page table and memzero
    ldr x0, =pg_dir
    lsl x0,x0,#16
    lsr x0,x0,#16
 
    add  x1, x0, #PAGE_SIZE // PUD's page frame
    add  x2, x1, #PAGE_SIZE  // 1st PMD's page frame
    add  x14, x2, #PAGE_SIZE // 2nd PMD's page frame
    add  x15, x14, #PAGE_SIZE // 1st PTE's page frame
                              // totally 1024 page (2G)

    // set PGD 
    ldr x3, = MM_TYPE_PAGE_TABLE // means it is a table descriptor
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]
    
    // set PUD
    ldr x3, = MM_TYPE_PAGE_TABLE
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
             
    ldr x3, = MM_TYPE_PAGE_TABLE 
    orr x3, x14, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD


    // set PMD
    mov x4, x15 //x4 for PTE's page table
    mov x5, xzr
    mov x6, #1024
3:
    ldr x3, = MM_TYPE_PAGE_TABLE
    orr x3, x4, x3
    str x3, [x2, x5, lsl #3] // +=8
    add x5, x5, #1    
    add x4, x4, #PAGE_SIZE //next PTE page
    cmp x5, x6 
    b.ls 3b

    // set PTE
    mov x4, xzr //x4 for physical memory
    mov x5, xzr
    mov x6, #0x41000000  
1:     
    ldr x3, = MMU_PTE_FLAGS2
    orr x3, x4, x3
    str x3, [x15, x5, lsl #3] // +=8
    add x5, x5, #1
    add x4, x4, #PAGE_SIZE 
    cmp x4, x6 
    b.ls 1b
 
    // adrp	x0, pg_dir
    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
 
    mov x30,x29
    ret