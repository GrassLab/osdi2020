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

    ldr x0, =__stack_top
    mov sp, x0

    // becuase virtual memory
    // so we cannot bl memzero here
    // we need set the virtual memory first
    //ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    //msr	sctlr_el1, x0


#if defined(RUN_ON_EL0) || defined(RUN_ON_EL1)

    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0

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

    ldr x0, =exception_table
    msr vbar_el1, x0

    // beg mmu
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, = BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.

    ldr x2, =boot_rest // indirect branch to the virtual address

    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
    mrs x1, sctlr_el1
    orr x1 , x1, 1
    msr sctlr_el1, x1 // enable MMU, cache remains disabled

    br x2

boot_rest:
    // end mmu

    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

    //b hang
    bl uart_init
    bl test
    bl init_irq

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
