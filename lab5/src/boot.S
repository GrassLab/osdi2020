#include "mm.h"
#include "mmu.h"
#include "armmmu.h"
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
    ldr x0, =__stack_top
    mov sp, x0

    b kernel_entry
#endif

el1_entry:

    ldr x0, =__stack_top
    mov sp, x0

#if MLV == 0

    adrp	x0, pg_dir
	  adrp	x1, pg_dir
    add x0, x0, 0x0000 // PGD's page frame at 0x0
    add x1, x1, 0x1000 // PUD's page frame at 0x1000

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

	  adrp	x0, pg_dir
	  adrp	x1, pg_dir
	  adrp	x2, pg_dir
	  adrp	x3, pg_dir
    add x0, x0, 0x0000 // PGD's page frame at 0x0
    add x1, x1, 0x1000 // PUD's page frame at 0x1000
    add x2, x2, 0x2000 // 1st PMD's page frame at 0x2000
    add x3, x3, 0x3000 // 2nd PMD's page frame at 0x4000

    ldr x4, =LV3_PGD_ATTR
    orr x4, x1, x4 // combine the physical address of next level page with attribute.
    str x4, [x0]

    ldr x4, =LV3_PUD_ATTR
    orr x5, x2, x4 // combine the physical address of next level page with attribute.
    str x5, [x1]
    orr x5, x3, x4 // combine the physical address of next level page with attribute.
    str x5, [x1, #8]

#if 0 /* all device */
    ldr x4, =LV3_PMD_ATTR
    mov x5, #512        // physical addr boundary
    mov x6, xzr         // physical addr counter 0 to 1023
    mov x7, x2          // entry address offset
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b
#else /* dev + normal */
    ldr x4, =LV3_PMD_NOR_ATTR
    mov x5, #504        // physical addr boundary
    mov x6, xzr         // physical addr counter 0 to 1023
    mov x7, x2          // entry address offset
1:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 1b

    ldr x4, =LV3_PMD_DEV_ATTR
    mov x5, #512        // physical addr boundary
2:
    mov x8, x6, lsl #21 // (0 to 1023) << 20 convert to MB
    orr x8, x8, x4
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls 2b
#endif
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

    bl init_irq

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    sub	x1, x1, x0
    bl  memzero


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
