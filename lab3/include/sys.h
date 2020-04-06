#ifndef	_SYS_H
#define	_SYS_H
void handle_sync(unsigned long esr, unsigned long address);
void handle_el0_sync(unsigned long esr, unsigned long address);

#endif  /*_SYS_H */