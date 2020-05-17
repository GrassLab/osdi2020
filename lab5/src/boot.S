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

#elif MLV == 1
    mov x0, 0x0000 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000
    mov x2, 0x2000 // PMD's page frame at 0x2000

    ldr x3, =LV3_PGD_ATTR
    orr x3, x1, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]

    ldr x3, =LV3_PUD_ATTR
    orr x3, x2, x3 // combine the physical address of next level page with attribute.
    str x3, [x1]

#if 0 /* all device */
    ldr x3, =LV3_PMD_ATTR
    mov x4, #512        // physical addr boundary
    mov x5, xzr         // physical addr counter 0 to 1023
    mov x6, x2          // entry address offset
1:
    mov x7, x5, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x7, x7, x3
    str x7, [x6], #8
    add x5, x5, #1
    cmp x5, x4
    b.ls 1b
#else /* dev + normal */
    ldr x3, =LV3_PMD_NOR_ATTR
    mov x4, #504        // physical addr boundary
    mov x5, xzr         // physical addr counter 0 to 1023
    mov x6, x2          // entry address offset
1:
    mov x7, x5, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x7, x7, x3
    str x7, [x6], #8
    add x5, x5, #1
    cmp x5, x4
    b.ls 1b

    ldr x3, =LV3_PMD_DEV_ATTR
    mov x4, #512        // physical addr boundary
1:
    mov x7, x5, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x7, x7, x3
    str x7, [x6], #8
    add x5, x5, #1
    cmp x5, x4
    b.ls 1b
#endif

#elif MLV == 2

    //ldr x0, =pg_dir
    //add x1, x0, 0x1000
    //add x2, x1, 0x1000
    //add x3, x2, 0x1000
    //add x4, x3, 0x1000

    mov x0, 0x0000 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000
    mov x2, 0x2000 // 1st PMD's page frame at 0x2000
    mov x3, 0x3000 // 2nd PMD's page frame at 0x3000
    mov x4, 0x4000 // PTD's page frame at 0x4000

    ldr x5, =LV4_PGD_ATTR
    orr x5, x1, x5 // combine the physical address of next level page with attribute.
    str x5, [x0]

    ldr x5, =LV4_PUD_ATTR
    orr x5, x2, x5 // combine the physical address of next level page with attribute.
    str x5, [x1]
    orr x5, x3, x5 // combine the physical address of next level page with attribute.
    str x5, [x1, #8]

    ldr x5, =LV4_PMD_ATTR
    mov x6, #512
    mov x7, xzr
    mov x8, x2
    mov x9, x4
1:
    orr x10, x9, x5
    str x10, [x8], #8
    add x7, x7, #1
    add x9, x9, #4096
    cmp x7, x6
    b.ls 1b

    ldr x5, =LV4_PTD_ATTR
    mov x6, #262144
    mov x7, xzr
    mov x8, x4
2:
    mov x9, x7, lsl #12
    orr x9, x9, x5
    str x9, [x8], #8
    add x7, x7, #1
    cmp x7, x6
    b.ls 2b
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

