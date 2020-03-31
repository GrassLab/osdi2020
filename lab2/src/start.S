_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
    ldr x0, = __bss_start
    ldr x1, = __bss_end

init_bss:
    str xzr, [x0]
    add x0, x0, #8
    cmp x0, x1
    ble init_bss

    ldr x0, = _start
    mov sp, x0
    bl main
1:
    b 1b
