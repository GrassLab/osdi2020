#ifndef EXCEP_HANDLER
#define EXCEP_HANDLER

//#define CORE0_TIMER_IRQ_CTRL (volatile unsigned int*)0x40000040

//extern int reschedule; // define in kernel.h

//extern struct task_struct; // define in kernel.h

//extern struct task_struct* get_current(); // define in context_switch.S

void excep_handler(unsigned long long sp);

//void excep_handler();

void el1_irq_isr();

void el0_irq_isr();

#endif
