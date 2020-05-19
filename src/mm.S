#include "../include/sysregs.h"
#include "../include/mm.h"
#include "../include/mmu.h"

// #define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
// #define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
// #define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)
// .global set_TCR_EL1
// set_TCR_EL1:
//     ldr x0, = TCR_CONFIG_DEFAULT
//     msr tcr_el1, x0
//     ret

// #define MAIR_DEVICE_nGnRnE 0b00000000
// #define MAIR_NORMAL_NOCACHE 0b01000100
// #define MAIR_IDX_DEVICE_nGnRnE 0
// #define MAIR_IDX_NORMAL_NOCACHE 1

// .global set_MAIR_EL1
// set_MAIR_EL1:
//     ldr x0, =( \
//     (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
//     (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
//     )
//     msr mair_el1, x0
//     ret

// #define PD_TABLE 0b11
// #define PD_BLOCK 0b01
// #define PD_ACCESS (1 << 10)
// #define BOOT_PGD_ATTR PD_TABLE
// #define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

// .global set_identity_mapping
// set_identity_mapping:
//     mov x0, 0 // PGD's page frame at 0x0
//     mov x1, 0x1000 // PUD's page frame at 0x1000

//     ldr x2, = BOOT_PGD_ATTR
//     orr x2, x1, x2 // combine the physical address of next level page with attribute.
//     str x2, [x0]

//     ldr x2, = BOOT_PUD_ATTR
//     mov x3, 0x00000000
//     orr x3, x2, x3
//     str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
//     mov x3, 0x40000000
//     orr x3, x2, x3
//     str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

//     msr ttbr0_el1, x0 // load PGD to the buttom translation based register.

//     mrs x2, sctlr_el1
//     orr x2 , x2, 1
//     msr sctlr_el1, x2 // enable MMU, cache remains disabled
//     ret

/*
 ** https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson06/src/boot.S
 */

.macro	create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
.endm

.macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1			// table index
	add	\tmp2, \tbl, #PAGE_SIZE
	orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE	
	str	\tmp2, [\tbl, \tmp1, lsl #3]
	add	\tbl, \tbl, #PAGE_SIZE					// next level table page
.endm

.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	lsr	\start, \start, #SECTION_SHIFT
	and	\start, \start, #PTRS_PER_TABLE - 1			// table index
	lsr	\end, \end, #SECTION_SHIFT
	and	\end, \end, #PTRS_PER_TABLE - 1				// table end index
	lsr	\phys, \phys, #SECTION_SHIFT
	mov	\tmp1, #\flags
	orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT			// table entry
9999:	str	\phys, [\tbl, \start, lsl #3]				// store the entry
	add	\start, \start, #1					// next entry
	add	\phys, \phys, #SECTION_SIZE				// next block
	cmp	\start, \end
	b.ls	9999b
.endm

.global __create_page_tables
__create_page_tables:
	mov	x29, x30						// save return address

	adrp	x0, pg_dir
	mov	x1, #PG_DIR_SIZE
	bl 	memzero

	adrp	x0, pg_dir
	mov		x1, #VA_START 
	create_pgd_entry x0, x1, x2, x3

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0
	mov 	x2, #VA_START						// first virtual address
	ldr		x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + DEVICE_BASE)				// first virtual address
	ldr		x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	adrp	x0, pg_dir				
	msr	ttbr1_el1, x0
    msr ttbr0_el1, x0

	mov	x30, x29						// restore return address
	ret








.global test__create_page_tables
test__create_page_tables:
	mov	x29, x30						// save return address

	adrp	x0, pg_dir
	mov	x1, #PG_DIR_SIZE
	bl 	memzero

	adrp	x0, pg_dir
	mov	x1, #VA_START 
	// create_table_entry x0, x1, PGD_SHIFT, x2, x3
	lsr	x2, x1, #PGD_SHIFT
	and	x2, x2, #PTRS_PER_TABLE - 1			// table index
	add	x3, x0, #PAGE_SIZE
	orr	x3, x3, #MM_TYPE_PAGE_TABLE	
	str	x3, [x0, x2, lsl #3]
	add	x0, x0, #PAGE_SIZE					// next level table page
	// create_table_entry x0, x1, PUD_SHIFT, x2, x3
	lsr	x2, x1, #PUD_SHIFT
	and	x2, x2, #PTRS_PER_TABLE - 1			// table index
	add	x3, x0, #PAGE_SIZE
	orr	x3, x3, #MM_TYPE_PAGE_TABLE	
	str	x3, [x0, x2, lsl #3]
	add	x0, x0, #PAGE_SIZE					// next level table page
	

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0
	mov 	x2, #VA_START						// first virtual address
	ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + DEVICE_BASE)				// first virtual address
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	adrp	x0, pg_dir				
	msr	ttbr1_el1, x0
    msr ttbr0_el1, x0


	mov	x30, x29						// restore return address
	ret








.globl memcpy
memcpy:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	b.gt memcpy
	ret

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret



.globl get_pgd
get_pgd:
	mov x1, 0
	ldr x0, [x1]
	mov x0, 0x1000
	msr	ttbr0_el1, x0
	ldr x0, [x1]
	ret


.globl set_pgd
set_pgd:
	dsb ish
	isb
	msr	ttbr0_el1, x0
	tlbi vmalle1is // invalidate all TLB entries
  	dsb ish        // ensure completion of TLB invalidatation
	isb            // clear pipeline
	ret