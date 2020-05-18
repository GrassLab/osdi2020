.globl memzero
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret

.globl set_pgd
set_pgd:
    msr	ttbr0_el1, x0
    tlbi vmalle1is
    DSB ISH              // ensure completion of TLB invalidation
    isb
    ret


.globl get_pgd
get_pgd:
    mov x1, 0
    ldr x0, [x1]
    mov x0, 0x1000
    msr	ttbr0_el1, x0
    ldr x0, [x1]
    ret
