#ifndef	_BOOT_H
#define	_BOOT_H

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );

extern unsigned int get_timer_freq();
extern unsigned long long get_timer_counts();
extern unsigned long get_reg();

void print_entry();
void print_exit();
#endif  /*_BOOT_H */
