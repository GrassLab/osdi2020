.section ".text"
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbnz x0, loop

    ldr x0, = _start
    mov sp, x0
    ldr     x1, =__bss_start
    ldr x2, =__bss_end
	b cmp_adr
set_zero:
    str xzr, [x1]
    add x1, x1, #8
cmp_adr:
    cmp x2, x1
    bhi set_zero

    // load exception_table to vbar_el2
    ldr x0, =exception_table
    msr vbar_el2, x0

    // enable interrupt
    mrs x0, hcr_el2
    orr x0, x0, #(1 << 4)
    msr hcr_el2, x0

jump_main:
    bl      main

loop:
    wfe
    b loop