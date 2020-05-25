#include "mm.h"

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
  mov x29, lr
  bl init_kernel_page_tables
  mov lr, x29

  adrp x0, page_table_start
  msr ttbr0_el1, x0
  msr ttbr1_el1, x0

  ret

/* Create page tables to support identity paging in kernel space */
.global init_kernel_page_tables
init_kernel_page_tables:
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