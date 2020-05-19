#ifndef __MMU_H__
#define __MMU_H__

// SCTLR_EL1, System Control Register (EL1)

#define SCTLR_VALUE_MMU_DISABLED 0
#define SCTLR_VALUE_MMU_ENABLE   1

// TCR_EL1, Translation Control Register

#define TTBR0_EL1_REGION_BIT    48
#define TTBR1_EL1_REGION_BIT    48
#define TTBR0_EL1_GRANULE       0b00 // 4KB
#define TTBR1_EL1_GRANULE       0b10 // 4KB

#define TCR_EL1_T0SZ            ((64 - TTBR0_EL1_REGION_BIT) << 0)
#define TCR_EL1_T1SZ            ((64 - TTBR1_EL1_REGION_BIT) << 16)
#define TCR_EL1_TG0             (TTBR0_EL1_GRANULE << 14)
#define TCR_EL1_TG1             (TTBR1_EL1_GRANULE << 30)
#define TCR_EL1_VALUE           (TCR_EL1_T0SZ | TCR_EL1_T1SZ | TCR_EL1_TG0 | TCR_EL1_TG1)

// MAIR_EL1, Memory Attribute Indirection Register

#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100 // Normal memory, Outer Non-cacheable | Normal memory, Inner Non-cacheable
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_VALUE              ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))

// Page Descriptor

#define PD_TABLE                0b11
#define PD_BLOCK                0b01
#define PD_PAGE                 0b11
#define PD_ACCESS_FLAG          (1 << 10)
#define PD_ACCESS_PERM_RW       (0b01 << 6)
#define PD_EXN                  (0b11UL << 53)
#define PD_MASK                 0x1FFUL

#define PGD0_ATTR               PD_TABLE // Lower attributes is ignored
#define PUD0_ATTR               PD_TABLE // Lower attributes is ignored
#define PUD1_ATTR               (PD_ACCESS_FLAG | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define PMD0_ATTR               PD_TABLE // Lower attributes is ignored
#define PTE_MMIO_ATTR           (PD_ACCESS_FLAG | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
#define PTE_RAM_ATTR            (PD_ACCESS_FLAG | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

#define PGD_SHIFT               39
#define PUD_SHIFT               30
#define PMD_SHIFT               21
#define PTE_SHIFT               12

#endif
