#ifndef _UTILS_H
#define _UTILS_H

extern void branch_to_address(void *);
extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);
extern int get_el(void);

#endif
