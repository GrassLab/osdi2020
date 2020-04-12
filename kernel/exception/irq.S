.section ".text"

.global irq_setup
.global irq_enable
.global irq_disable
.global core_timer_enable
.global core_timer_handler

irq_setup:

    # irq init (HCR_EL2.IMO
    mrs x0, hcr_el2
    orr x0, x0, #0b10000
    msr hcr_el2, x0

    # return
    ret

irq_enable:
    msr daif, xzr
    ret

irq_disable:
    mov x0, #0b1111000000
    msr daif, x0
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040
core_timer_enable:

    mov x0, #1

    # Core Timer Interrupt
    # bit[0]: nCNTPSIRQ:    secure physical timer
    # bit[1]: nCNTPNSIRQ:   Non-secure physical timer
    # bit[2]: nCNTHPIRQ:    Hypervisor Timer Event
    # bit[3]: nCNTVIRQ:     Virtual Timer Event
    # bit[4]: nCNTPSIRQ:    FIQ
    # bit[5]: nCNTPNSIRQ:   FIQ
    # bit[6]: nCNTHPIRQ:    FIQ
    # bit[7]: nCNTVIRQ:     FIQ

    # enable timer
    msr cntp_ctl_el0, x0 
    mov x0, #0b00000010
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    
    # enable timer interrupt
    str x0, [x1] 
    
    ret

#define EXPIRE_PERIOD 0xFFFFFFF
core_timer_handler:
    mov x0, EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    ret