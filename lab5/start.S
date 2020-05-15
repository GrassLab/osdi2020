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
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

# define page_size (1<<21)

.section ".text"
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbnz x0, loop

setup_EL1:
    mov x0, (1 << 31) // EL1 use aarch64
    msr hcr_el2, x0

    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0

    adr x0, jump_main
    msr elr_el2, x0

    # setup EL1 stack
    adr x0, _start
    msr sp_el1, x0

    mrs x0, CPACR_EL1   // let va_list can use
    orr x0, x0, #(3 << 20)
    msr CPACR_EL1, x0
    eret

jump_main:
    adr x1, __bss_start
    adr x2, __bss_end
	b cmp_adr
set_zero:
    str xzr, [x1]
    add x1, x1, #8 

cmp_adr:
    cmp x2, x1
    bhi set_zero
    
    bl set_vm 

    ldr x0, = TCR_CONFIG_DEFAULT // set TCR_EL1
    msr tcr_el1, x0

    // set MAIR
    ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    msr DAIFClr, 0xf // enable irq

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    ldr x0, =main

    adr x1, exception_table  // set exception_table
    msr vbar_el1, x1

    br x0

loop:
    wfe
    b loop

set_vm:
    mov x0, 0 // PGD's page frame at 0x0
    add x1, x0, page_size // PUD's page frame
    add x2, x1, page_size // PMD's page 1 frame
    add x3, x2, page_size // PMD's page 2 frame

    // set PGD
    ldr x6, = BOOT_PGD_ATTR
    orr x4, x6, x1 // combine the physical address of next level page with attribute.
    str x4, [x0]

    // set PUD
    ldr x6, = BOOT_PUD_ATTR
    orr x4, x6, x2 
    str x4, [x1] // 1st 1GB mapped by the 1st entry of PUD

    ldr x6, = BOOT_PUD_ATTR
    orr x4, x6, x3
    str x4, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // set PMD
    mov x1, #0 // x1 page_offset
    mov x4, #0
    mov x5, #1024 // PMD = 512 * 2 (page1 and 2)

set_PMD:
    ldr x6, = BOOT_PMD_ATTR
    orr x7, x6 , x1
    str x7, [x2 , x4, lsl #3]
    add x1 , x1 , page_size
    add x4, x4, #1
    cmp x4, x5
    b.ls set_PMD

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
    ret