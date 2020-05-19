#ifndef __MMU_MMU_H
#define __MMU_MMU_H

// --------------------------------------
// - TCR (Translation Control Register)
// --------------------------------------
// setup [5:0] (T0SZ) and [21:16] (T1SZ) in TCR, the value in TxSZ field means
// most significant {value} bits has to be 0s or 1s.
#define TCR_CONFIG_REGION_48_BIT (((64 - 48) << 0) | ((64 - 48) << 16))

// setup [15:14] (TG0) and [31:30] (TG1) in TCR, the value in TGx field means
// translation granule, 00=4KB, 01=16KB, 11=64KB
#define TCR_CONFIG_PAGE_4_KB ((0b00 << 14) | (0b10 << 30))

#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48_BIT | TCR_CONFIG_PAGE_4_KB)

// ------------------------------------------------
// - MAIR (Memory Attribute Indirection Register)
// ------------------------------------------------
#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

// ------------------------------------------------
// - Page xxxx Directory
// ------------------------------------------------
#define PD_TABLE 0b11
#define PD_BLOCK 0b01

// Access Flag @ [10]
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#define PAGE_SIZE 0x1000

#endif
