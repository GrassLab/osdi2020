.section ".text.boot"

.global _start
_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF         // get CPU ID and mask reserved bits
    cbz     x0, clear_bss

pe_hang:                          // hang for all non-primary CPU
    wfe                           // enter low power mode
    b       pe_hang

clear_bss:
    ldr     w1, =__bss_size
    cbz     w1, pseudo_main
    ldr     x0, =__bss_start
    bl      memzero

// set_sp:
    ldr     x1, =_start
    mov     sp, x1

pseudo_main:
    bl      main                  // jump to C code, should not return
    b       pe_hang               // for failsafe, halt this core too