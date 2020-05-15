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
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR PD_TABLE
#define BOOT_PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | 0b11)
#define BOOT_PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | 0b11)

#define PAGE_SIZE 4096

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

  /* Initialize page tables */
  mov x29, x30
  bl init_page_tables
  mov x30, x29

  adrp x0, page_table_start
  msr ttbr0_el1, x0
  msr ttbr1_el1, x0

  ret

.global init_page_tables
init_page_tables:
  /* Setup PGD */
  adrp x0, page_table_start
  adrp x1, page_table_start
  add x1, x1, PAGE_SIZE
  ldr x2, =BOOT_PGD_ATTR
  orr x2, x1, x2
  str x2, [x0]

  /* Setup PUD */
  add x0, x0, PAGE_SIZE
  add x1, x1, PAGE_SIZE
  ldr x2, =BOOT_PUD_ATTR
  orr x1, x1, x2
  str x1, [x0]
  add x1, x1, PAGE_SIZE
  str x1, [x0, 8]

  /* Setup PMD */
  add x0, x0, PAGE_SIZE
  add x1, x1, PAGE_SIZE
  mov x3, 2
1:
  sub x3, x3, #1
  mov x4, #512
2:
  sub x4, x4, #1
  ldr x2, =BOOT_PMD_ATTR
  orr x2, x1, x2
  str x2, [x0]
  add x0, x0, #8
  add x1, x1, PAGE_SIZE
  cbnz x4, 2b
  cbnz x3, 1b

  /* Setup PTE for normal region */
  mov x1, xzr
  mov x3, #(0x3f000000 / (512 * 4096))
3:
  sub x3, x3, #1
  mov x4, #512
4:
  sub x4, x4, #1
  ldr x2, =BOOT_PTE_NORMAL_ATTR
  orr x2, x1, x2
  str x2, [x0]
  add x0, x0, #8
  add x1, x1, PAGE_SIZE
  cbnz x4, 4b
  cbnz x3, 3b

  /* Setup PTE for MMIO region */
  mov x3, #(512 - (0x3f000000 / (512 * 4096)) + 512)
5:
  sub x3, x3, #1
  mov x4, #512
6:
  sub x4, x4, #1
  ldr x2, =BOOT_PTE_DEVICE_ATTR
  orr x2, x1, x2
  str x2, [x0]
  add x0, x0, #8
  add x1, x1, PAGE_SIZE
  cbnz x4, 6b
  cbnz x3, 5b

  ret