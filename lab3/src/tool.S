.section ".text"

.global branch_to_address
branch_to_address:
    br x0
    ret // We never reach this point
