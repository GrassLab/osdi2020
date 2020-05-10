/*
 * T0SZ, bits [5:0]:   The size offset of the memory region addressed by TTBR0_EL1
 *                     The region size is 2^(64-T0SZ) bytes
 * T1SZ, bits [21:16]: The size offset of the memory region addressed by TTBR1_EL1
 *                     The region size is 2^(64-T1SZ) bytes
 * TG1, bits [15:14]:  Set page size for the TTBR1_EL0, 0b00 means 4KB
 * TG1, bits [31:30]:  Set page size for the TTBR1_EL1, 0b10 means 4KB
 *
 * https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/tcr_el1
 */
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) | (0b10 << 30))
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

.global vm_init
vm_init:
  ldr x0, =TCR_CONFIG_DEFAULT
  msr tcr_el1, x0

  /*
   *   +-------+-------+-------+-------+
   *   | Attr7 |  ...  | Attr1 | Attr0 |
   *   +-------+-------+-------+-------+
   *          56      16       8       0
   */
  ldr x0, =(                                               \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0

  mov x0, 0
  mov x1, 0x1000

  ldr x2, =BOOT_PGD_ATTR
  orr x2, x1, x2
  str x2, [x0]

  ldr x2, =BOOT_PUD_ATTR
  mov x3, 0x00000000
  orr x3, x2, x3
  str x3, [x1]
  mov x3, 0x40000000
  orr x3, x2, x3
  str x3, [x1, 8]

  msr ttbr0_el1, x0

  /* Enable MMU for EL1 and EL0 stage 1 address translation. */
  mrs x2, sctlr_el1
  orr x2, x2, 1
  msr sctlr_el1, x2

  ret
