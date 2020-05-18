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
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define PAGE_SIZE (0x1000) //4096

.section ".text.boot"

.global _start

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
    
1:  wfe
    b 1b
2:  
    ldr x0, =exception_table
    msr vbar_el1, x0

    ldr x1, =_start // init sp for el1 option 1 (kernel stack)
    msr sp_el1, x1

    mov x0, (1 << 31) // EL1 use aarch64
    msr hcr_el2, x0

    mov x0, 0x5 // EL1h (SPSel = 1) with interrupt disabled, origin 0x3c5
    msr spsr_el2, x0
    adr x0, rest_initialization // load exception return address
    msr elr_el2, x0
    eret

.global rest_initialization
rest_initialization:
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    adr x0, _pgt_start // PGD's page frame at 0x0
    add x1, x0, PAGE_SIZE // PUD's page frame at 0x1000
    add x3, x1, PAGE_SIZE


    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]


    mov x4, x3
    ldr x2, = BOOT_PUD_ATTR
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD

    add x3, x3, PAGE_SIZE
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // free to use x1, x2, x3, x5, x6, x7
    mov x1, 0
    mov x2, 1023
    ldr x3, = BOOT_PMD_ATTR
3:
    mov x5, x1, lsl #21 // x5 = x1*0x200000
    orr x6, x3, x5
    str x6, [x4, x1, lsl #3]
    add x1, x1, 1
    sub x6, x2, x1
    cbnz x6, 3b


    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    ldr x2, =boot_rest // indirect branch to the virtual address
    br x2


.global boot_rest
boot_rest:
    ldr x1, =__bss_start //clear bss
    ldr w2, =__bss_size
5:  cbz w2, 6f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 5b
6:
    bl kernel_init
    b 1b
    
