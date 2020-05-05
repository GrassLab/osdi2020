.section "text"

.global mem_init

// to set up tcr_el1
#define TCR_CONFIG_REGION_48bit ( ( ( 64 - 48 ) << 0 ) | ( ( 64 - 48 ) << 16 ) )
#define TCR_CONFIG_4KB          ( ( 0b00 << 14 ) | ( 0b10 << 30 ) )
#define TCR_CONFIG_DEFAULT      ( TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB )

// to set up mair_el1
#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1

# https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/tcr_el1
mem_init:
    # set up tcr_el1
    ldr     x0,     =TCR_CONFIG_DEFAULT
    msr     TCR_EL1,    x0

    # set  up mair_el1
    
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0