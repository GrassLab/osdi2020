#ifndef	_IRQ_H
#define	_IRQ_H


void irq_vector2_init( void );
void irq_vector1_init( void );
void enable_irq( void );
void disable_irq( void );
void exception_rount_el2( void );
void core_timer_enable( void );
#endif  /*_IRQ_H */
