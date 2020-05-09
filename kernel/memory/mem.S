.section "text"

.global mem_init

// to set up tcr_el1
#define TCR_CONFIG_REGION_48bit ( ( ( 64 - 48 ) << 0 ) | ( ( 64 - 48 ) << 16 ) )    // bits[4] = bits[20] = 1
#define TCR_CONFIG_4KB          ( ( 0b00 << 14 ) | ( 0b10 << 30 ) )                 // bits[31] = 1
#define TCR_CONFIG_DEFAULT      ( TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB )

// to set up mair_el1
#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1

// to set up PGD PUD
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

mem_init:
    # set up tcr_el1
    # https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/tcr_el1
    # bits[31:30] - Granule size for the TTBR1_EL1. 0b10, 4KB
    # bits[21:16] - T1SZ - The size offset of the memory region addressed by TTBR1_EL1.
    # bits[5:0] - T0SZ - The size offset of the memory region addressed by TTBR0_EL1. 
    # TTBR1_EL1 - https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/ttbr1_el1
    ldr     x0,     =TCR_CONFIG_DEFAULT
    msr     TCR_EL1,    x0

    # set up mair_el1
    # # https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/mair_el1
    ldr     x0, =( \
                (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
                (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
            )
    msr     MAIR_EL1,   x0

    # set up identity mapping
    # set up PGD PUD
    mov     x0,     0               // PGD's page frame at 0x0
    mov     x1,     0x1000          // PUD's page frame at 0x1000

    ldr     x2,     =BOOT_PGD_ATTR
    orr     x2,     x1,     x2      // combine the physical address of next level page with attribute.
    str     x2,     [x0]

    ldr     x2,     =BOOT_PUD_ATTR
    mov     x3,     0x00000000
    orr     x3,     x2,     x3
    str     x3,     [x1]            // 1st 1GB mapped by the 1st entry of PUD
    mov     x3,     0x40000000
    orr     x3,     x2,     x3
    str     x3,     [x1, 8]         // 2nd 1GB mapped by the 2nd entry of PUD

    msr     TTBR0_EL1, x0           // load PGD to the buttom translation based register.

    mrs     x2,     SCTLR_EL1
    orr     x2 ,    x2,     1
    msr     SCTLR_EL1, x2           // enable MMU, cache remains disabled