#ifndef _SYSREGS_H
#define _SYSREGS_H

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of
// AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN (0 << 24)
#define SCTLR_I_CACHE_DISABLED (0 << 12)
#define SCTLR_D_CACHE_DISABLED (0 << 2)
#define SCTLR_MMU_DISABLED (0 << 0)
#define SCTLR_MMU_ENABLED (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED                                               \
    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED |        \
     SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of
// AArch64-Reference-Manual.
// ***************************************

#define HCR_RW (1 << 31)
#define HCR_VALUE HCR_RW

// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of
// AArch64-Reference-Manual.
// ***************************************

#define SCR_RESERVED (3 << 4)
#define SCR_RW (1 << 10)
#define SCR_NS (1 << 0)
#define SCR_VALUE (SCR_RESERVED | SCR_RW | SCR_NS)

// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of
// AArch64-Reference-Manual.
// ***************************************

#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h (5 << 0)
#define SPSR_VALUE (SPSR_MASK_ALL | SPSR_EL1h)

// ***************************************
// SPSR_EL1
// ***************************************

#define EL1_SPSR_MASK (1 << 6)
#define EL1_SPSR_EL0 (15 << 0)
#define EL1_SPSR_VALUE (EL1_SPSR_MASK & ~EL1_SPSR_EL0)

// ***************************************
// ESR_EL1, Exception Syndrome Register (EL1). Page 2431 of AArch64-Reference-Manual.
// ***************************************

#define ESR_ELx_EC_SHIFT        26
#define ESR_ELx_EC_SVC64        0x15

// ***************************************
// TCR_EL1, Translation Control Register
// ***************************************
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB          ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT      (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

// ***************************************
// MAIR_EL1, Memory Attribute Indirection Register
// ***************************************
#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_CONFIG_DEFAULT     (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8))

// ***************************************
// TTBR
// ***************************************
#define PD_TABLE            0b11
#define PD_BLOCK            0b01
#define PD_PAGE             0b11
#define PD_ACCESS           (1 << 10) 

#define PGD_ATTR            PD_TABLE
#define PUD_ATTR            PD_TABLE
#define PMD_ATTR            PD_TABLE

#define NORMAL_MEM_FLAGS    (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)
#define DEVICE_MEM_FLAGS    (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2)  | PD_PAGE)

#endif
