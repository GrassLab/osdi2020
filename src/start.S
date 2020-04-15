.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, _id0
    // cpu id > 0, stop
_buzy_loop:
    wfe
    b       _buzy_loop

_id0:
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size

_clear_bss:
    cbz     x2, _set_exc_table
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, _clear_bss

_set_exc_table:
	ldr x0, =exception_table	//set exception table
	msr VBAR_EL2, x0

    // jump to relocated C code, should not return
    bl      main
    // for failsafe, halt this core too
    b       _start
