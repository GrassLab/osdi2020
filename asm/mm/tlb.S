#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_PAGE 0b11
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define PD_PERIPHERAL (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
#define PD_RAM (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

.section ".text"
.global setup_va
setup_va:
  // set 4K page and 48bit address space
  ldr x0, = TCR_CONFIG_DEFAULT
  msr tcr_el1, x0

  // device memory for peripheral, normal memory for ram
  ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0

  // allocate 2MB for kernel
  // ffff000000000000-ffff000000200000 -> 000000000000-000000200000
  mov x0, 0x0         // PGD
  mov x1, 0x1000      // PUD
  mov x2, 0x2000      // PMD
  mov x3, 0x3000      // PTE

  ldr x4, =PD_TABLE
  orr x4, x1, x4
  str x4, [x0, 0x8 * 0x0] // put PUD into PGD

  ldr x4, =PD_TABLE
  orr x4, x2, x4
  str x4, [x1, 0x8 * 0x0] // put PMD into PUD

  ldr x4, =PD_TABLE
  orr x4, x3, x4
  str x4, [x2, 0x8 * 0x0] // put PTE into PMD

  // setup PTE entries
  mov x5, 0x0          // for (x5 = 0; x5 < 0x200; ++x5)
loop:
  cmp x5, 0x200
  b.ge end_loop
  lsl x6, x5, #12    // x6 = 4KB block
  ldr x4, =PD_RAM
  orr x4, x6, x4     // set attribute
  lsl x6, x5, #3
  str x4, [x3, x6]   // put 4KB block into PTE
  add x5, x5, 1	     // ++x5
  b loop
end_loop:

  msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
  msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

  mrs x2, sctlr_el1
  orr x2 , x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled

  ret
