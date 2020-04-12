#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#define CORE0_TIMER_IRQ_CTRL ((volatile uint32_t *)0x40000040)

extern unsigned vector_table[];

void exception_init(void);
void curr_el_spx_sync_handler(void);

#endif // EXCEPTION_H_
