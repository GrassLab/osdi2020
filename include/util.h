#ifndef	__UTIL_H__
#define	__UTIL_H__

extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);
extern unsigned int get_cntfrq();
extern unsigned int get_cntpct();

#endif
