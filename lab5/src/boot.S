#include "mm.h"
#include "mmu.h"
#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, continue

hang:
    wfe
    b hang

continue:

    //ldr x0, =__stack_top
    //mov sp, x0

    // becuase virtual memory
    // so we cannot bl memzero here
    // we need set the virtual memory first
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0

#if defined(RUN_ON_EL0) || defined(RUN_ON_EL1)

    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry
    msr	elr_el2, x0

    eret
#else
    b kernel_entry
#endif

el1_entry:

    ldr x0, =__stack_top
    mov sp, x0

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    sub	x1, x1, x0
    bl  memzero

    bl init_irq

#define MLV 1
#if MLV == 0
    /* beg */
    // use physical address of page table
    mov x0, 0

    add  x1, x0,   #PAGE_SIZE // PUD's page frame
    add  x2, x1,   #PAGE_SIZE // 1st PMD's page frame
    add  x14, x2,  #PAGE_SIZE // 2nd PMD's page frame
    add  x15, x14, #PAGE_SIZE // 1st PTE's page frame
                              // totally 1024 page (2G)

    // step 1. setting PGD
    ldr x3, =PD_TABLE // means it is a table descriptor
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]

    // step 2. setting PUD
    ldr x3, =PD_TABLE
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD

    ldr x3, =PD_TABLE
    orr x3, x14, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // step 3. setting PMD
    mov x4, x15 //x4 for PTE's page table
    mov x5, xzr
    mov x6, #1024
3:
    ldr x3, =PD_TABLE
    orr x3, x4, x3
    str x3, [x2, x5, lsl #3]
    add x5, x5, #1
    add x4, x4, #PAGE_SIZE //next PTE page
    cmp x5, x6
    b.ls 3b

    // step4. setting PTE
    mov x4, xzr //x4 for physical memory
    mov x5, xzr
    mov x6, #0x41000000
1:
    ldr x3, =MMU_DEVICE_FLAGS
    orr x3, x4, x3
    str x3, [x15, x5, lsl #3]
    add x5, x5, #1
    add x4, x4, #PAGE_SIZE
    cmp x4, x6
    b.ls 1b

    /* end */
#elif MLV == 1

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000
    mov x2, 0x2000 // PUD's page frame at 0x1000

    ldr x3, =PGD_ATTR
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]

    ldr x3, =PUD_ATTR
    orr x3, x2, x3 // combine the physical address of next level page with attribute.
    str x3, [x1]

    mov x5, xzr
    mov x6, #1024
1:
    ldr x3, =PMD_ATTR
    mov x4, x5, lsl #20
    orr x4, x4, x3
    str x4, [x2, x5, lsl #3]
    add x5, x5, #1
    cmp x5, x6
    b.ls 1b

#elif MLV == 2
    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, =BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, =BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD
#endif

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    ldr x0, =exception_table
    msr vbar_el1, x0

    ldr x2, =boot_rest // indirect branch to the virtual address
    mrs x1, sctlr_el1
    orr x1 , x1, 1
    msr sctlr_el1, x1 // enable MMU, cache remains disabled

    br x2

boot_rest:
    // end mmu

#ifdef RUN_ON_EL1
    b kernel_entry
#endif

    ldr x0, 0
    msr spsr_el1, x0

    adr x0, el0_entry
    msr elr_el1, x0

    // goto el0
    eret

el0_entry:

    mov x0, #LOW_MEMORY
    mov sp, x0

kernel_entry:
    bl main

