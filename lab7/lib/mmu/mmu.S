.section ".text.mmu"

#include "mmu/mmu.h"

.global init_mmu

init_mmu:
    mov x29, x30 // save return address since we're going to bl to another function

    ldr x0, = TCR_CONFIG_DEFAULT
    msr TCR_EL1, x0

    // put in attr0 (0 * 8) and attr1 (1 * 8)
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr MAIR_EL1, x0

    bl create_page_tables

    mrs x2, SCTLR_EL1
    orr x2, x2, 1
    msr SCTLR_EL1, x2 // enable MMU, cache remains disabled

    mov x30, x29

    ret

// 2 MB (2 ** 21)
#define GRANULARITY (1 << 21)

// x0: attr, x1: page_table_entry, x2: physical_start, x3: physical_end
create_block_map:
    sub x3, x3, x2
    mov x4, x3

    orr x2, x0, x2
body:
    str x2, [x1], #8
    add x2, x2, #GRANULARITY
    subs x4, x4, #GRANULARITY
    bgt body

    ret

create_page_tables:
    mov x28, x30

    mov x0, 0 // PGD's page frame
    mov x1, 0x1000 // PUD's page frame
    mov x2, 0x2000 // PMD's page frame

    ldr x3, =BOOT_PGD_ATTR
    // combine the physical address of next level page table with attribute
    orr x3, x1, x3
    str x3, [x0]

    ldr x3, =BOOT_PUD_ATTR
    // combine the physical address of next level page table with attribute
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x2, 0x3000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    ldr x0, =BOOT_PMD_RAM_ATTR
    mov x1, 0x2000 // PMD's page frame
    mov x2, 0x00000000
    mov x3, 0x3F000000
    bl create_block_map

    ldr x0, =BOOT_PMD_PERIPHERAL_ATTR
    mov x2, 0x3F000000
    mov x3, 0x80000000
    bl create_block_map

    mov x0, 0 // PGD's page frame
    msr TTBR0_EL1, x0 // load PGD to the buttom translation based register
    msr TTBR1_EL1, x0 // load PGD to the upper translation based register

    mov x30, x28

    ret
