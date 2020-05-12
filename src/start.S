.section ".text.boot"
.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    // cpu id == 0
    cbnz     x1, loop

    // clear bss
__bss_clear:
    adr	x0, __bss_start
	adr	x1, __bss_end
	sub	x1, x1, x0
	bl 	memzero


_setup_el1:
    // check if run in EL2
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits //lsr x0, x0, #2

    // set up exception handlers
    ldr     x0, =_exception_table
    msr     vbar_el1, x0

    // set stack before our code
    ldr     x1, =_start
    msr     sp_el1, x1

    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    
    // change execution level to EL1
    mov     x0, 0x5//#0x3c4
    msr     spsr_el2, x0
    adr     x0, _run_main  // from el2 return to el1
    msr     elr_el2, x0
    eret


// #define USER_STACK 0x1000

// _setup_el0:
//     // disable exception of access SIMD register in EL1
//     mrs     x0, cpacr_el1
//     orr     x0, x0, #(3 << 20)
//     msr     cpacr_el1, x0

//     // setup EL0 stack
//     mov     x1, USER_STACK
//     msr     sp_el0, x1
//     // switch to EL0
//     // spsr_el1 clear daif and switch to EL0
//     mov     x1, 0x0
//     msr     spsr_el1, x1
//     adr     x1, _run_main  // from el1 return to el0
//     msr     elr_el1, x1
//     eret


#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

_run_main:
    bl __create_page_tables

    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0


    // mov x0, 0 // PGD's page frame at 0x0
    // mov x1, 0x1000 // PUD's page frame at 0x1000

    // ldr x2, = BOOT_PGD_ATTR
    // orr x2, x1, x2 // combine the physical address of next level page with attribute.
    // str x2, [x0]

    // ldr x2, = BOOT_PUD_ATTR
    // mov x3, 0x00000000
    // orr x3, x2, x3
    // str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    // mov x3, 0x40000000
    // orr x3, x2, x3
    // str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    // msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    // msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    // mrs x2, sctlr_el1
    // orr x2 , x2, 1
    // msr sctlr_el1, x2 // enable MMU, cache remains disabled


    // mov	x0, x0, #LOW_MEMORY

	adrp	x0, pg_dir				
	msr	ttbr1_el1, x0
    msr ttbr0_el1, x0

	// ldr	x0, =(TCR_VALUE)		
	// msr	tcr_el1, x0

	// ldr	x0, =(MAIR_VALUE)
	// msr	mair_el1, x0

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    // mov	x0, 1//#SCTLR_MMU_ENABLED				
	// msr	sctlr_el1, x0

    // // set up stack
    // ldr     x0, =_start
    // mov     sp, x0

    // jump to C code, should not return
    // bl      main
    ldr	x2, =main
    br x2
loop:  
    // for failsafe, halt this core too
    // cpu id > 0, stop
    wfe             // wait for event 
    b       loop


