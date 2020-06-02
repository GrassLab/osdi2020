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

#define EL1_SPSR_MASK       (1 << 6)
#define EL1_SPSR_EL0		(15 << 0)
#define EL1_SPSR_VALUE      (EL1_SPSR_MASK & ~EL1_SPSR_EL0)

#endif