#ifndef _ASM_H
#define _ASM_H

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern int get_el ( void );
extern void enable_irq(void);
extern void disable_irq(void);
extern void asm_delay( void );

#endif
