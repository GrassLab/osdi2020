.global mmu_tcr_el1_init
.type mmu_tcr_el1_init,%function
mmu_tcr_el1_init:

  // tcr_el1: The control register for stage 1 of the EL1&0 translation regime

  /* Setup T0SZ[5:0] and T1SZ[21:16] */
  /* The size offset of the memory region addressed by TTBRx_EL1 */
  /* The region size is 2 ** (64-TxSZ) bytes. */
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))

  /* Setup TG0[15:14] and TG1[31:30] */
  /* Granule size for TTBRx_EL1 */
  /* 0b00 -> 4KB */
  /* 0b01 -> 64KB */
  /* 0b10 -> 16KB */
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))

#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

  ldr x0, = TCR_CONFIG_DEFAULT
  msr tcr_el1, x0
  ret

.global mmu_mair_el1_init
.type mmu_mair_el1_init,%function
mmu_mair_el1_init:

  /* mair_el1: Provides the memory attribute encodings corresponding to the possible AttrIndx values in a Long-descriptor format translation table entry for stage 1 translations at EL1. */

	/*
  Gathering (G/nG)
  - Determines whether multiple accesses can be merged into a single bus transaction
  - nG: Number/size of accesses on the bus = number/size of accesses in code
  Reordering (R/nR)
  - Determines whether accesses to the same device can be reordered
  - nR: Accesses to the same IMPLEMENTATION DEFINED block size will appear on the bus in program order
  Early Write Acknowledgement (E/nE)
  - Indicates to the memory system whether a buffer can send acknowledgements
  - nE: The response should come from the end slave, not buffering in the interconnect
  */
  /*
  Device memory: 0b0000dd00
  0b00 | Device-nGnRnE memory
  0b01 | Device-nGnRE memory
  0b10 | Device-nGRE memory
  0b11 | Device-GRE memory
  */
#define MAIR_DEVICE_nGnRnE 0b00000000

  /*
  Normal memory: 0booooiiii, (oooo != 0000 and iiii != 0000)

	o:
	0b0000	See encoding of Attr
	0b00RW, RW not 0b00	Normal memory, Outer Write-Through Transient
	0b0100	Normal memory, Outer Non-cacheable
	0b01RW, RW not 0b00	Normal memory, Outer Write-Back Transient
	0b10RW	Normal memory, Outer Write-Through Non-transient
	0b11RW	Normal memory, Outer Write-Back Non-transient

	i:
	0b0000	See encoding of Attr
	0b00RW, RW not 0b00	Normal memory, Inner Write-Through Transient
	0b0100	Normal memory, Inner Non-cacheable
	0b01RW, RW not 0b00	Normal memory, Inner Write-Back Transient
	0b10RW	Normal memory, Inner Write-Through Non-transient
	0b11RW	Normal memory, Inner Write-Back Non-transient

	RW:
	0b0	No Allocate
	0b1	Allocate
  */
#define MAIR_NORMAL_NOCACHE 0b01000100

#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

  ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0
  ret

.global mmu_ttbr0_ttbr1_el1_init
.type mmu_ttbr0_ttbr1_el1_init,%function
mmu_ttbr0_ttbr1_el1_init:

  /* ttbr0_el1: Holds the base address of the translation table for the initial lookup for stage 1 of the translation of an address from the lower VA range in the EL1&0 translation regime, and other information for this translation regime. */

  /* PD (ttbrx) -> PGD -> PUD -> PMD -> | Page + offset */

#define PD_TABLE 0b11 // indicate this to another descriptor
#define PD_BLOCK 0b01 // indicate to physical ram
#define PD_ACCESS (1 << 10) // access flag, generate page fault if not set
#define BOOT_PGD_ATTR PD_TABLE
// Set access bit and mair[4:2] and indicate to physical ram
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

  mov x0, 0 // PGD's page frame at 0x0
  mov x1, 0x1000 // PUD's page frame at 0x1000

  ldr x2, = BOOT_PGD_ATTR
  orr x2, x1, x2 // combine the physical address of next level page with attribute.
  str x2, [x0]

  ldr x2, = BOOT_PUD_ATTR
  mov x3, 0x00000000
  orr x3, x2, x3
  str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
  mov x3, 0x40000000
  orr x3, x2, x3
  str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

  // ttbr0 and ttbr1 share the same PUD frame
  msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
  msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
  ret

.global mmu_enable_mmu
.type mmu_enable_mmu,%function
mmu_enable_mmu:
  /* Provides top level control of the system, including its memory system, at EL1 and EL0 */
  /* M[0]: MMU enable for EL1 and EL0 stage 1 address translation */

  mrs x2, sctlr_el1
  orr x2 , x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled
  ret

