#ifndef _MMU_H
#define _MMU_H


/*
 * Memory region attributes:
 *   DEVICE_encode  0000 xx00 P.B2-143
 *   NORMAL_encode  (outer | inner) P.B2-147
 *      outer: xxxx ----
 *      inner: ---- xxxx
 */
#define MAIR_DEVICE_nGnRnE            0b00000000// Device non-Gathering, non-Reordering, No Early write acknowledgement.
#define MAIR_NORMAL_NCACHE            0b01000100// Normal memory, Outer Non-cacheable, Inner Non-cacheable

#define MAIR_DEVICE_nGnRnE_IDX        0         // store device memory setting in mair attr0
#define MAIR_NORMAL_NCACHE_IDX        1         // store normal memory setting in mair attr1
#define MAIR_ATTR_LEN                 8         // each mair attribute is a 8-bits entry
#define MAIR_VALUE                    (MAIR_DEVICE_nGnRnE << MAIR_DEVICE_nGnRnE_IDX * MAIR_ATTR_LEN |\
                                       MAIR_NORMAL_NCACHE << MAIR_NORMAL_NCACHE_IDX * MAIR_ATTR_LEN)


/*
 * Translation Control Register
 * The control register for stage 1 of the EL1&0 translation regime.
 */
// num of signficant used
#define TCR_T0SZ                      ((64-48) <<  0)// The size offset of the memory region addressed by TTBR1_EL1. The region size is 2^(64-T1SZ) bytes.
#define TCR_T1SZ                      ((64-48) << 16)// The size offset of the memory region addressed by TTBR0_EL1. The region size is 2^(64-T0SZ) bytes.
#define TCR_TG0_4K                    (0b00 << 14)   // Granule size for the TTBR0_EL1.
#define TCR_TG0_64K                   (0b01 << 14)   // Granule size for the TTBR0_EL1.
#define TCR_TG0_16K                   (0b10 << 14)   // Granule size for the TTBR0_EL1.
#define TCR_TG1_16K                   (0b01 << 30)   // Granule size for the TTBR1_EL1.
#define TCR_TG1_4K                    (0b10 << 30)   // Granule size for the TTBR1_EL1.
#define TCR_TG1_64K                   (0b11 << 30)   // Granule size for the TTBR1_EL1.
#define TCR_VALUE                     (TCR_TG1_4K | TCR_TG0_4K | TCR_T1SZ | TCR_T0SZ)

/*
 * Page table entry(descriptor) attributes
 * PGD = level0
 * PUD = level1
 * PMD = level2
 * PTE = level3
 */
#define PT_DESC_INVALID               0b0        // Entry is invalid when bit0 set to zero.
#define PT_DESC_TO_BLOCK              0b01       // This descriptor is valid and store an address to block.
#define PT_DESC_TO_TABLE              0b11       // This descriptor is valid and store an address to next level page table(Note: level 0 should always set to this).
#define PT_DESC_TO_PAGE               0b11       // This descriptor is valid and store an address to a page(Note: level 3 should always set to this).
// Lower attributes
#define PT_DESC_AP_PRIVI_RW           (0b00 << 6)// Access permission.
#define PT_DESC_AP_UNPRI_RW           (0b01 << 6)// Access permission.
#define PT_DESC_AP_PRIVI_R            (0b10 << 6)// Access permission.
#define PT_DESC_AP_UNPRI_R            (0b11 << 6)// Access permission.
#define PT_DESC_AF                    (0b1 << 10)// Access flag.
//Upper attributes
#define PT_DESC_PXN                   (0b1 << 53)// Privileged eXecute Never 
#define PT_DESC_UXN                   (0b1 << 54)// Uprivileged eXecute Never

#define PT_DEV_LOWER_ATTR             (PT_DESC_AF | MAIR_DEVICE_nGnRnE_IDX<<2)
#define PT_NOR_LOWER_ATTR             (PT_DESC_AF | MAIR_NORMAL_NCACHE_IDX<<2)
#define PT_DEV_UPPER_ATTR             (PT_DESC_UXN | PT_DESC_PXN)
#define PT_NOR_UPPER_ATTR             (0x0)


#endif//_MMU_H
