.globl irq_vector2_init
irq_vector2_init:
	adr	x0, vectors		// load VBAR_EL2 with virtual
	msr	vbar_el2, x0		// vector table address
	ret

.globl irq_vector1_init
irq_vector1_init:
	adr	x0, vectors		// load VBAR_EL1 with virtual
	msr	vbar_el1, x0		// vector table address
	ret

.globl enable_irq
enable_irq:
	msr    daifclr, 0xf 
	ret

.globl disable_irq
disable_irq:
	msr	daifset, 0xf
	ret
	
.globl exception_rount_el2
exception_rount_el2:
	mrs	x0, CurrentEL	       // check currentEL 
	cmp	x0, 0x8                // 0xC(EL3)、0x8(EL2)、0x4(EL1)、0x0(EL0)
	b.ne	err_hang	       // currentEL != EL1 
	mrs	x9, hcr_el2            // hcr_el2(Hypervisor Configuration Register)
    orr x9, x9, #0x10            /*el2: HCR_EL2.IMO set */
	msr	hcr_el2, x9
    ret


#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffffff
.globl core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable timer
    mov x0, EXPIRE_PERIOD
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1] // enable timer interrupt
    ret

.globl core_timer_handler
core_timer_handler:
    mov x0, EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    ret

