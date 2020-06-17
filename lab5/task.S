// TCR_EL1
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)
// MAIR_EL1
#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
// identity mapping
#define PD_TABLE 0b11
#define PD_PAGE  0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define PD_ACCESS_PERMISSION	(1 << 6) 
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR PD_TABLE
#define BOOT_PTE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)

# define page_size (1<<12)

.global switch_to
switch_to:
  mov x9, sp
  stp x19, x20, [x0], #16     // store callee-saved registers
  stp x21, x22, [x0], #16
  stp x23, x24, [x0], #16
  stp x25, x26, [x0], #16
  stp x27, x28, [x0], #16
  stp x29, x9, [x0], #16
  str x30, [x0]

  ldp x19, x20, [x1], #16     // restore callee-saved registers
  ldp x21, x22, [x1], #16
  ldp x23, x24, [x1], #16
  ldp x25, x26, [x1], #16
  ldp x27, x28, [x1], #16
  ldp x29, x9, [x1], #16
  ldr x30, [x1]
  mov sp, x9
  ret

.global get_current
get_current:
  mrs x0, tpidr_el1
  ret

.global set_current
set_current:
  msr tpidr_el1, x0
  ret

.globl ret_from_fork
ret_from_fork:
  mov	x29, 0
  mov x0, 0
  blr x19         //should never return

.global ret_to_usr
ret_to_usr:
  ldp x0, x1, [sp ,16 * 0]
  ldp x2, x3, [sp ,16 * 1]
  ldp x4, x5, [sp ,16 * 2]
  ldp x6, x7, [sp ,16 * 3]
  ldp x8, x9, [sp ,16 * 4]
  ldp x10, x11, [sp ,16 * 5]
  ldp x12, x13, [sp ,16 * 6]
  ldp x14, x15, [sp ,16 * 7]
  ldp x16, x17, [sp ,16 * 8]
  ldp x18, x19, [sp ,16 * 9]
  ldp x20, x21, [sp ,16 * 10]
  ldp x22, x23, [sp ,16 * 11]
  ldp x24, x25, [sp ,16 * 12]
  ldp x26, x27, [sp ,16 * 13]
  ldp x28, x29, [sp ,16 * 14]
  ldr x30, [sp, 16 * 15]
  add sp, sp, 32 * 8
  mov x0,0
  eret

.global set_pagetable
set_pagetable:
    add x1, x0, #100 // PUD's page frame
    add x2, x1, #100 // PMD's page 1 frame
    add x3, x2, #100 // PMD's page 2 frame
    add x8, x3, #100 // PTE's page1  frame
    // set PGD
    ldr x6, = BOOT_PGD_ATTR
    orr x6, x1, x6 // combine the physical address of next level page with attribute.
    str x6, [x0]

    // set PUD
    ldr x6, = BOOT_PUD_ATTR
    orr x6, x2, x6 
    str x6, [x1] // 1st 1GB mapped by the 1st entry of PUD

    ldr x6, = BOOT_PUD_ATTR
    orr x6, x3, x6
    str x6, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // set PMD
    mov x1, x8 // x1 page_offset
    mov x4, xzr
    mov x5, #1024 // PMD = 512 * 2 (page1 and 2)

3:
    ldr x6, = BOOT_PMD_ATTR
    orr x6, x1 , x6
    str x6, [x2 , x4, lsl #3]
    add x1 , x1 , #page_size
    add x4, x4, #1
    cmp x4, x5
    b.ls 3b

     // set PTE
    mov x1 , xzr
    mov x4, xzr
    mov x5, #0x100
1:
    ldr x6, = BOOT_PTE_ATTR
    orr x6, x1 , x6
    str x6, [x8, x4, lsl #3]
    add x1 , x1 , #page_size
    add x4, x4, #1
    cmp x4, x5
    b.ls 1b
  ret