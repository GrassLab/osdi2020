.globl memzero_64
memzero_64:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero_64
    ret