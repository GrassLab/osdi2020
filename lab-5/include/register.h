#ifndef REGISTER_H
#define REGISTER_H

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

#define HCR_RW	    			(1 << 31)
#define HCR_VALUE			    HCR_RW

#define SPSR_MASK_ALL 			(0xf << 6)
#define SPSR_EL1h			    (5 << 0)
#define SPSR_VALUE			    (0x3c5)  // EL1h (SPSel = 1) with interrupt disabled

#define USER_STACK              (0x40000)
#define KERNEL_STACK            (0x80000)

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

#define MMU_FLAGS	 		        (MM_TYPE_PAGE | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS)	
#define MMU_DEVICE_FLAGS		    (MM_TYPE_PAGE | (MAIR_IDX_DEVICE_nGnRnE << 2) | MM_ACCESS)	
#define MMU_PTE_FLAGS			    (MM_TYPE_PAGE | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)	


#define PT_DESC_PXN                   (0b1 << 53)// Privileged eXecute Never 
#define PT_DESC_UXN                   (0b1 << 54)// Uprivileged eXecute Never
#define PT_DESC_AF                    (0b1 << 10)// Access flag.
#define PT_DESC_TO_BLOCK              0b01      // This descriptor is valid and store an address to block.

#define PT_PMD_DEV_LOWER_ATTR         (PT_DESC_AF |  (MAIR_IDX_DEVICE_nGnRnE << 2) )
#define PT_PMD_NOR_LOWER_ATTR         (PT_DESC_AF |  (MAIR_IDX_NORMAL_NOCACHE << 2) )
#define PT_PMD_DEV_UPPER_ATTR         (PT_DESC_UXN | PT_DESC_PXN)
#define PT_PMD_NOR_UPPER_ATTR         (0x0)
#define PT_PMD_DEV_ATTR               (PT_PMD_DEV_LOWER_ATTR | PT_DESC_TO_BLOCK)
#define PT_PMD_NOR_ATTR               (PT_PMD_NOR_UPPER_ATTR | PT_PMD_NOR_LOWER_ATTR | PT_DESC_TO_BLOCK)

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
// #define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK) 
#define BOOT_PMD_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK) 

#define MMU_FLAGS (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_TABLE)
#define MMU_DEVICE_FLAGS   (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_TABLE)

#endif