#include "mm.h"
.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0


el2_entry:

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    msr     sp_el1, x1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // change execution level to EL1
    mov     x2, #0x344
    msr     spsr_el2, x2
    adr     x2, el1_entry
    msr     elr_el2, x2

    eret

el1_entry:  
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el1, x2

    ldr     x0, 0
    msr     spsr_el1, x0

setup_pages:
    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    ldr x0, =_pgd_base // PGD's page frame at 0x0
    ldr x1, =_pud_base
    mov x2, x0 

    // setup PGD
    ldr x3, =PD_TABLE
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x2]

    ldr x1, =_pmd_base
    ldr x2, =_pud_base

    // setup first PUD
    ldr x3, =PD_TABLE
    orr x3, x1, x3
    str x3, [x2]

    add x1, x1, #PUD_BLOCK_SIZE

    // setup second PUD
    ldr x3, =BOOT_PUD_DEVICE_ATTR
    mov x8, 0x40000000
    orr x3, x8, x3
    str x3, [x2, 8]

    ldr x1, =_pte_base
    ldr x2, =_pmd_base

    // setup PMD x 512
    mov x4, xzr
    mov x5, #PMD_AMOUNT
3:
    ldr x3, =PD_TABLE
    orr x3, x1, x3
    str x3, [x2]
    add x2, x2, #PAGE_SIZE

    add x1, x1, #PMD_BLOCK_SIZE

    add x4, x4, #1
    cmp x4, x5
    b.lt 3b

    ldr x2, =_pte_base

    // setup PTE x 512
    mov x4, xzr
    mov x5, #0x40000000
1:
    ldr x3, =(PD_TABLE | PD_ACCESS)
    orr x3, x4, x3
    str x3, [x2]
    add x2, x2, #PAGE_SIZE
    mov x8, #(1<<12)
    add x4, x4, x8
    cmp x4, x5
    b.ls 1b

    msr ttbr0_el1, x0
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
    mrs x2, sctlr_el1
    orr x2, x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

clear_bss:
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b


tran:
    ldr x2, =boot_rest // indirect branch to the virtual address
    br x2

boot_rest:


go_main:
    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
1:
    b       1b

go_el0:


el0_entry:

