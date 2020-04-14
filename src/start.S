.section ".text.boot"

.global _start

_start:
    // save arguments in registers (we will need them later for the new kernel)
    // I choosed x10-x13 because instructions generated from C by gcc does not
    // touch them. You can check that with "aarch64-elf-objdump -d kernel8.elf"
    mov     x10, x0
    mov     x11, x1
    mov     x12, x2
    mov     x13, x3

    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, _id0
    // cpu id > 0, stop
_buzy_loop:
    wfe
    b       _buzy_loop

_id0:
    // relocate our code from load address to link address
    ldr     x1, =0x80000
    ldr     x2, =_start
    ldr     w3, =__loader_size
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

_clear_bss:
    cbz w2, _set_exc_table
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, _clear_bss

_set_exc_table:
	ldr x0, =exception_table	//set exception table
	msr VBAR_EL2, x0

    // jump to relocated C code, should not return
    bl      main
    // for failsafe, halt this core too
    b       _start
