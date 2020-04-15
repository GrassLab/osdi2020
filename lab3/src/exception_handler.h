#ifndef EXCEP_HANDLER
#define EXCEP_HANDLER

#define CORE0_TIMER_IRQ_CTRL (volatile unsigned int*)0x40000040

void handler();

void isr();

#endif
