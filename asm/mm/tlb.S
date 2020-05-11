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
.global pd_encode_table
pd_encode_table:
  ldr x1, =PD_TABLE
  orr x0, x0, x1
  ret
.global pd_encode_ram
pd_encode_ram:
  ldr x1, =PD_RAM
  orr x0, x0, x1
  ret
.global pd_encode_peripheral
pd_encode_peripheral:
  ldr x1, =PD_PERIPHERAL
  orr x0, x0, x1
  ret
.global mmu_enable
mmu_enable:
  msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
  msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

  // set 4K page and 48bit address space
  ldr x0, =TCR_CONFIG_DEFAULT
  msr tcr_el1, x0

  // device memory for peripheral, normal memory for ram
  ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0

  mrs x2, sctlr_el1
  orr x2 , x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled

  ret
