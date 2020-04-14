#ifndef	_EXCE_H
#define	_EXCE_H

#include "peri_base.h"

#define CORE0_IRQ_SRC        ((volatile unsigned int*)0x40000060)

void sync_svc_handler(unsigned long esr, unsigned long elr);
void enable_irq( void );
void disable_irq( void );
#endif  /*_EXC_H */