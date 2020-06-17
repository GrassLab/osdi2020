#define USER_STACK 0x1000

.section ".text"
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbnz x0, loop

    ldr x0, = _start
    mov sp, x0
initial_bss:
    ldr     x1, =__bss_start
    ldr x2, =__bss_end
	b cmp_adr
set_zero:
    str xzr, [x1]
    add x1, x1, #8
cmp_adr:
    cmp x2, x1
    bhi set_zero

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
    msr DAIFClr, 0xf // enable irq
    ldr x0, =exception_table  // set exception_table
    msr vbar_el1, x0
    bl      main

loop:
    wfe
    b loop