#ifndef _MMU_H_
#define _MMU_H_

#define PAGESIZE 4096
#define PAGE (PAGESIZE / 8)

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

#define TTBR_CNP 1
#define PTE_SHIFT 12

void mmu_init();

unsigned long pa_to_pfn(unsigned long pa);

unsigned long virtual_to_physical(unsigned long vir);
#endif
