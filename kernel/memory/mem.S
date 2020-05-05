.section "text"

.global mem_init

# https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/tcr_el1
mem_init:
    ldr     x0,     =TCR_CONFIG_DEFAULT
    msr     TCR_EL1,    x0