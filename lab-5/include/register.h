#ifndef REGISTER_H
#define REGISTER_H

#define HCR_RW	    			(1 << 31)
#define HCR_VALUE			    HCR_RW

#define SPSR_MASK_ALL 			(0xf << 6)
#define SPSR_EL1h			    (5 << 0)
#define SPSR_VALUE			    (0x3c5)  // EL1h (SPSel = 1) with interrupt disabled

#define USER_STACK              (0x40000)
#define KERNEL_STACK            (0x80000)

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#endif