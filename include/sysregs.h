#ifndef __SYSREGS_H__
#define __SYSREGS_H__

// SCTLR_EL1, System Control Register (EL1)

#define SCTLR_VALUE_MMU_DISABLED 0

// HCR_EL2, Hypervisor Configuration Register

#define HCR_EL2_RW_AARCH64  (1 << 31)
#define HCR_EL2_VALUE       (HCR_EL2_RW_AARCH64)

// SPSR_EL2, Saved Program Status Register (EL2)

#define SPSR_EL2_MASK_ALL   (0b0000 << 6)
#define SPSR_EL2_EL1h       (0b0101 << 0)
#define SPSR_EL2_VALUE      (SPSR_EL2_MASK_ALL | SPSR_EL2_EL1h)

// SPSR_EL1, Saved Program Status Register (EL1)

#define SPSR_EL1_MASK       (0b0000 << 6)
#define SPSR_EL1_EL0        (0b0000 << 0)
#define SPSR_EL1_VALUE      (SPSR_EL1_MASK | SPSR_EL1_EL0)

// CPACR_EL1, Architectural Feature Access Control Register

#define CPACR_EL1_FPEN      (0b11 << 20)
#define CPACR_EL1_VALUE     (CPACR_EL1_FPEN)

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

#endif
