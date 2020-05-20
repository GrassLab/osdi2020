#ifndef _MU_H
#define _MU_H

#define VA_START 			0xffff000000000000
#define PHYS_MEMORY_SIZE 		0x40000000	

#define PBASE 0x3F000000
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)
#define HIGH_MEMORY PBASE

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#define THREAD_SIZE PAGE_SIZE

#define PAGESIZE 4096
#define PAGE (PAGESIZE / 8)


/** addressible region is 48bit **/
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
/** Page granule size is 4KB **/
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

/**  Device-nGnRnE memory **/
#define MAIR_DEVICE_nGnRnE 0b00000000
/** Normal memory, Outer Non-cacheable **/
/** Normal memory, Inner Non-cacheable **/
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_CONFIG_DEFAULT  (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) 


// granularity
#define PT_PAGE 0b11  // 4k granule
#define PT_BLOCK 0b01 // 2M granule

#define PT_PGD 0b11 // 2M granule
#define PT_PUD 0b01 // 2M granule
#define PT_PMD 0b01 // 2M granule
#define PT_PTE 0b11 // 2M granule
// accessibility
#define PT_KERNEL (0 << 6) // privileged, supervisor EL1 access only
#define PT_USER (1 << 6)   // unprivileged, EL0 access allowed
#define PT_RW (0 << 7)     // read-write
#define PT_RO (1 << 7)     // read-only
#define PT_AF (1 << 10)    // accessed flag
#define PT_NX (1UL << 54)  // no execute
// shareability
#define PT_OSH (2 << 8) // outter shareable
#define PT_ISH (3 << 8) // inner shareable
// defined in MAIR register
#define PT_MEM (0 << 2) // normal memory
#define PT_DEV (1 << 2) // device MMIO
#define PT_NC (2 << 2)  // non-cachable


// granularity
#define PT_TABLE 0b11
#define PT_PAGE 0b11
#define PT_BLOCK 0b01

#define BOOT_PGD_ATTR PT_TABLE
#define BOOT_PUD_ATTR (PT_AF | (MAIR_IDX_DEVICE_nGnRnE << 2) | PT_BLOCK)
#define BOOT_PMD_ATTR (PT_AF | (MAIR_IDX_DEVICE_nGnRnE << 2) | PT_BLOCK)
#define BOOT_PTE_ATTR (PT_AF | PT_TABLE)



#define PD_TABLE 0b11
#define PD_PAGE  0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define PD_ACCESS_PERMISSION	(1 << 6) 

#define MMU_FLAGS (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)
#define MMU_DEVICE_FLAGS   (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
#define MMU_PTE_FLAGS   (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE | PD_ACCESS_PERMISSION)



#define TTBR_CNP 1


#endif