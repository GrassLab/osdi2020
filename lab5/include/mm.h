#ifndef	_MM_H
#define	_MM_H

#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY              	(2 * SECTION_SIZE)
#define EL0_LOW_MEMORY              (4 * SECTION_SIZE)

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE  0b11
#define PD_ACCESS (1 << 10)
#define PD_ACCESS_PERMISSION	(1 << 6)
#define PD_READONLY     (1 << 7)
#define PD_NON_EXEC_EL0 (1UL << 54)

#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR PD_TABLE
#define BOOT_PMD_BLOCK_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2))

#define BOOT_PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) )
#define BOOT_PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) )
#define PT_DESC_AP_PRIVI_RW           (0b00 << 6)// Access permission.

#define PT_DESC_PXN                   (0b1 << 53)// Privileged eXecute Never 
#define PT_DESC_UXN                   (0b1 << 54)// Uprivileged eXecute Never
#define PT_DESC_AF                    (0b1 << 10)// Access flag.
#define PT_DESC_TO_BLOCK              0b01       // This descriptor is valid and store an address to block.

#define PT_PMD_DEV_LOWER_ATTR         (PT_DESC_AF |  (MAIR_IDX_NORMAL_NOCACHE << 2) | PT_DESC_AP_PRIVI_RW)
#define PT_PMD_NOR_LOWER_ATTR         (PT_DESC_AF |  (MAIR_IDX_NORMAL_NOCACHE << 2) | PT_DESC_AP_PRIVI_RW)
#define PT_PMD_DEV_UPPER_ATTR         (PT_DESC_UXN | PT_DESC_PXN)
#define PT_PMD_NOR_UPPER_ATTR         (0x0)
#define PT_PMD_DEV_ATTR               (PT_PMD_DEV_UPPER_ATTR | PT_PMD_DEV_LOWER_ATTR | PT_DESC_TO_BLOCK)
#define PT_PMD_NOR_ATTR               (PT_PMD_NOR_UPPER_ATTR | PT_PMD_NOR_LOWER_ATTR | PT_DESC_TO_BLOCK)


#define PGD_ATTR PD_TABLE
#define PUD_ATTR PD_TABLE
#define PMD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);
void virt_mem_init();

#endif

#endif  /*_MM_H */