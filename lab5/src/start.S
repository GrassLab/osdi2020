#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR (PD_TABLE)
#define BOOT_PUD_ATTR (PD_TABLE)

#define BOOT_PMD_ATTR (PD_TABLE)
#define BOOT_PMD_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define BOOT_PMD_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#define BOOT_PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_TABLE)
#define BOOT_PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_TABLE)

_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
    adr x0, __bss_start
    adr x1, __bss_end

init_bss:
    str xzr, [x0]
    add x0, x0, #8
    cmp x0, x1
    ble init_bss

// load exception_table to VBAR_EL1
    ldr x0, =exception_table
    msr VBAR_EL1, x0
// from el2 to el1
    mov x0, (1 << 31)
    msr HCR_EL2, x0
    mov x0, 0x3c5
    msr SPSR_EL2, x0
    ldr x0, =kernel_init
    //ldr x0, =boot_rest
    msr ELR_EL2, x0
    ldr x0, =_start
    msr SP_EL1, x0
    
//test
//mov sp, x0

set_up_TCR_EL1:
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

set_up_MAIR_EL1:
    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    //bl enable_mmu
enable_mmu:
    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    mov x0, 0x1000
    mov x1, 0x2000

    ldr x2, = BOOT_PUD_ATTR
    orr x2, x1, x2
    str x2, [x0], #8 // 1st 1GB mapped by the 1st entry of PUD
    mov x1, 0x3000
    orr x2, x1, x2
    str x2, [x0]

    mov x0, 0x2000

    ldr x2, = BOOT_PMD_NORMAL_ATTR
    mov x3, #504
    mov x4, xzr
init_pmd_normal:
    lsl x5, x4, #21
    orr x5, x5, x2
    str x5, [x0], #8
    add x4, x4, #1
    cmp x4, x3
    b.ls init_pmd_normal

    ldr x2, = BOOT_PMD_DEVICE_ATTR
    mov x3, #512
init_pmd_device:
    lsl x5, x4, #21
    orr x5, x5, x2
    str x5, [x0], #8
    add x4, x4, #1
    cmp x4, x3
    b.ls init_pmd_device

    mov x0, 0x3000
    ldr x2, = BOOT_PMD_DEVICE_ATTR
    mov x3, #1024
    mov x4, #512
init_pmd_2gb_device:
    lsl x5, x4, #21
    orr x5, x5, x2
    str x5, [x0], #8
    add x4, x4, #1
    cmp x4, x3
    b.ls init_pmd_2gb_device

    mov x0, 0
    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // load PGD to the high translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    //ldr x2, =kernel_init
    //br x2
    //b kernel_init

    eret

//boot_rest:
    //b kernel_init

1:
    b 1b
