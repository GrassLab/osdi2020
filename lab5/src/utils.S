.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

.globl put32
put32:
    str w1,[x0]
    ret

.globl get32
get32:
    ldr w0,[x0]
    ret

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.globl set_pgd
set_pgd:
    dsb ish
    msr	ttbr0_el1, x0     // switch translation based address
    tlbi vmalle1is        // invalidate all TLB entries
    dsb ish               // ensure completion of TLB invalidation
    ret

.globl get_pgd
get_pgd:
    mov x1, 0
    ldr x0, [x1]
    mov x0, 0x1000
    msr	ttbr0_el1, x0
    ldr x0, [x1]
    ret
