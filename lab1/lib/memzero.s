.global memzero

memzero:
    // use zero register to set 0 and post-indexed with 8 bytes
    str xzr, [x0], #8
    subs x1, x1, #8
    bgt memzero
    ret
