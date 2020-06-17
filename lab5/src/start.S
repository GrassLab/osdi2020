#include "mm.h"
#include "gpio.h"
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
    mov     sp, #(2 * SECTION_SIZE) 

    // clear bss
    adr x0, __bss_start
    adr x1, __bss_end
    sub x1, x1, x0
    bl  memzero

    bl  __create_page_tables
   
    //running at el1 ?
6: 
    mrs x0, CurrentEL
    and x0, x0, #12 // clear reserved bits
    cmp     x0, #4
    bne     1b
    
    mov x0, #VA_START
    add sp, x0, #LOW_MEMORY 
    
    //setting TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0
    
    //setting MAIR
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0
/*
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
*/    


    ldr x0 ,=main   // ?
    
    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    br x0

    .macro  create_pgd_entry, tbl, virt, tmp1, tmp2
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
    .endm

    //allocating a new page table (PGD or PUD)
    .macro  create_table_entry, tbl, virt, shift, tmp1, tmp2
    // extracting table index 
    lsr \tmp1, \virt, #\shift                   // right shift first to strip everything to the right of the index 
    and \tmp1, \tmp1, #PTRS_PER_TABLE - 1       // (1 << 9) - 1 = '0b111111111',using and operation to strip everything to the left
    
    add \tmp2, \tbl, #PAGE_SIZE                 // tmp2 = the address of the next page table
    orr \tmp2, \tmp2, #MM_TYPE_PAGE_TABLE       // 2 lower bits set to 1,means this entry is point to a table
    
    // store in the current page table
    str \tmp2, [\tbl, \tmp1, lsl #3] 

    // tbl parameter to point to the next page table                   
    add \tbl, \tbl, #PAGE_SIZE                 
    .endm

    .macro  create_block_map, tbl, phys, start, end, flags, tmp1
    // extracting start(virtual address) table index
    lsr \start, \start, #SECTION_SHIFT
    and \start, \start, #PTRS_PER_TABLE - 1         // table index
    
    // extracting end(virtual address) table index
    lsr \end, \end, #SECTION_SHIFT
    and \end, \end, #PTRS_PER_TABLE - 1             // table end index
    
    lsr \phys, \phys, #SECTION_SHIFT                // ?
    mov \tmp1, #\flags
    orr \phys, \tmp1, \phys, lsl #SECTION_SHIFT         // table entry
9999:   str \phys, [\tbl, \start, lsl #3]               // store the entry
    add \start, \start, #1                  // next entry
    add \phys, \phys, #SECTION_SIZE             // next block
    cmp \start, \end
    b.ls    9999b                           //cmp lower or same
    .endm

__create_page_tables:
	mov	x29, x30						// save return address

	adrp	x0, pg_dir
	mov	x1, #PG_DIR_SIZE
	bl 	memzero

	adrp	x0, pg_dir
	mov	x1, #VA_START 
	create_pgd_entry x0, x1, x2, x3

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0(xzr:zero register)
	mov 	x2, #VA_START						// first virtual address
	ldr	x3, =(VA_START + PA_MMIO_BASE - SECTION_SIZE)		// last virtual address, PA_MMIO_BASE(=0x3F000000)
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #PA_MMIO_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + PA_MMIO_BASE)				// first virtual address
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address ?
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    adrp    x0, pg_dir
    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register
	
    mov	x30, x29						// restore return address
	ret

