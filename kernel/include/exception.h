#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#define SYSTEM_TIMER_CS ((volatile uint32_t *)0x3f003000)
#define SYSTEM_TIMER_CL0 ((volatile uint32_t *)0x3f003004)
#define SYSTEM_TIMER_C1 ((volatile uint32_t *)0x3f003010)
// https://wiki.osdev.org/BCM_System_Timer
#define SYSTEM_TIMER_FREQUENCY 1000000

#define IRQ_BASIC_PENDING ((volatile uint32_t *)0x3f00b200)
#define IRQ_PENDING1 ((volatile uint32_t *)0x3f00b204)
#define IRQ_PENDING2 ((volatile uint32_t *)0x3f00b208)
#define IRQ_ENABLE1 ((volatile uint32_t *)0x3f00b210)

// https://github.com/raspberrypi/documentation/blob/master/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
#define CORE0_TIMER_IRQ_CTRL ((volatile uint32_t *)0x40000040)
#define CORE0_INTERRUPT_SRC ((volatile uint32_t *)0x40000060)

extern unsigned vector_table[];

void exception_init(void);
void core_timer_enable(void);
void curr_el_spx_sync_handler(void);
void curr_el_spx_irq_handler(void);
void core_timer_handler(void);
void gpu_interrupt_handler(void);
void system_timer_handler(void);
void not_implemented_handler(void);

void enable_interrupt(void);
void disable_interrupt(void);

#endif // EXCEPTION_H_
