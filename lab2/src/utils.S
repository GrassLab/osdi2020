.global branch_to_address
branch_to_address:
    br x0

.global get32
get32:
    ldr w0, [x0]
    ret