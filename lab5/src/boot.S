#include "arm/mmu.h"
#include "arm/sysregs.h"
#include "mm.h"
#include "peripherals/base.h"

// PGD: shift 39
    // PUD: shift 30
    // ... etc
    .macro	create_pgd_entry, tbl, virt, tmp1, tmp2
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
    .endm

    // construct a page descriptor:
    //    Entry of PGD, PUD, PMD which point to a page table
    //
    //    +-----+------------------------------+---------+--+
    //    |     | next level table's phys addr | ignored |11|
    //    +-----+------------------------------+---------+--+
    //         47                             12         2  0
    //
    //    Entry of PUD, PMD which point to a block
    //
    //    +-----+------------------------------+---------+--+
    //    |     |  block's physical address    |attribute|01|
    //    +-----+------------------------------+---------+--+
    //         47                              n         2  0
    //
    //    Entry of PTE which point to a page
    //
    //    +-----+------------------------------+---------+--+
    //    |     |  page's physical address     |attribute|11|
    //    +-----+------------------------------+---------+--+
    //         47                             12         2  0
    //
    //    Invalid entry
    //
    //    +-----+------------------------------+---------+--+
    //    |     |  page's physical address     |attribute|*0|
    //    +-----+------------------------------+---------+--+
    //         47                             12         2  0
    .macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
    lsr	\tmp1, \virt, #\shift
    and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1  // table index
    add	\tmp2, \tbl, #PAGE_SIZE            // next level tabls' phys addr(next)
    orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE  // desc <- next | MM_TYPE_PAGE_TABLE(11)
    str	\tmp2, [\tbl, \tmp1, lsl #3]       // [tbl + index << 3] <- desc
    add	\tbl, \tbl, #PAGE_SIZE             // next level table page
    .endm

    .macro	create_block_map, tbl, phys, start, end, flags, tmp1
    // extract table start index & table end index
    lsr	\start, \start, #SECTION_SHIFT
    and	\start, \start, #PTRS_PER_TABLE - 1     // table index
    lsr	\end, \end, #SECTION_SHIFT
    and	\end, \end, #PTRS_PER_TABLE - 1         // table end index

    // create block descriptor
    lsr	\phys, \phys, #SECTION_SHIFT
    mov	\tmp1, #\flags
    orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT // table entry
9999:	str	\phys, [\tbl, \start, lsl #3]       // store the entry
    add	\start, \start, #1                      // next entry
    add	\phys, \phys, #SECTION_SIZE             // next block
    cmp	\start, \end
    b.ls	9999b
    .endm


.section ".text.boot"


// let core with cpuid != 0 enter busy loop
.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f

// cpuid > 0
.global halt
halt:
    wfe
    b halt

// cpuid = 0
2:
    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

    mov	x0, #VA_START
    add	sp, x0, #LOW_MEMORY


    // setup TCR
    ldr x0, =(TCR_VALUE)
    msr tcr_el1, x0

    // setup MAIR
    ldr x0, =(MAIR_VALUE)
    msr mair_el1, x0

    bl page_table_create


    // indirect branch
    ldr x3, =kernel_main

    mrs x2, sctlr_el1
    orr x2, x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

boot_rest:
    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry
    msr	elr_el2, x0

    eret

el1_entry:
    mov x0, #VA_START
    add sp, x0, #LOW_MEMORY

    // load exception table for el1
    ldr x0, =vectors
    msr vbar_el1, x0

    // enable interrupt
    bl  enable_interrupt_controller
    msr daifclr, #0xf

    // Lab4. start from el1_main
    br  x3
    b   halt

page_table_create:
    mov x29, x30
    adrp	x0, pg_dir
    adrp	x1, pg_dir
    adrp	x2, pg_dir
    adrp	x3, pg_dir

    add x0, x0, 0x0000 // PGD's page frame at 0x0
    add x1, x1, 0x1000 // PUD's page frame at 0x1000
    add x2, x2, 0x2000 // 1st PMD's page frame at 0x2000
    add x3, x3, 0x3000 // 2nd PMD's page frame at 0x4000


    ldr x4, =PD_TABLE
    // insert a pointer to PUD into  PGD
    orr x4, x1, x4 // combine the physical address of next level page with attribute.
    str x4, [x0]

    ldr x4, =PD_TABLE
    // populates the PUD table with pointer to PMD
    orr x5, x2, x4 // combine the physical address of next level page with attribute.
    str x5, [x1]
    orr x5, x3, x4 // combine the physical address of next level page with attribute.
    str x5, [x1, #8]



    // Algo:
    // input: attr, boundary, store_addr
    // ==============================
    // int i = 0;
    // do {
    //     unsigned long phy_addr = i << 20;
    //     phy_addr |= attr;
    //     i++;
    // } while (i < boundary)

    // setup
    ldr x4, =BOOT_PMD_ATTR
    mov x5, #504        // physical addr boundary (0x3F000000 / (2^21))
    mov x6, xzr         // physical addr counter 0 to 1023
    mov x7, x2          // 1st PMD, entry address offset
// populations
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 21 convert to MB
    orr x8, x8, x4      // combine attribute
    str x8, [x7], #8    // store to next address of x7
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b


    // setup
    ldr x4, =BOOT_PMD_DEV_ATTR
    mov x5, #512        // physical addr boundary
// populations
1:
    mov x8, x6, lsl #21
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b

    msr ttbr1_el1, x0 // load PGD to the upper translation based register.
    msr ttbr0_el1, x0 // load PGD to the bottom translation based register.

    mov x30, x29
    ret

__create_page_tables:
    mov	x29, x30						// save return address

    adrp	x0, pg_dir
    mov	x1, #PG_DIR_SIZE
    bl  memzero

    adrp x0, pg_dir
    mov	 x1, #VA_START
    create_pgd_entry x0, x1, x2, x3

    /* Mapping kernel and init stack*/
    mov x1, xzr                         // start mapping from physical offset 0
    mov x2, #VA_START                   // first virtual address
    ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)    // last virtual address
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4      ;

    /* Mapping device memory*/
    mov     x1, #DEVICE_BASE					// start mapping from device base address
    ldr     x2, =(VA_START + DEVICE_BASE)				// first virtual address
    ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    msr	ttbr1_el1, x0
    msr	ttbr0_el1, x0


    mov	x30, x29						// restore return address
    ret
