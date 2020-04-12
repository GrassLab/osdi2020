#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 2
#define CORE_TIMER    0   // syscal numbers
#define SYS_TIMER     1

#ifndef __ASSEMBLER__
int core_timer();
void sys_timer();
void print_this(unsigned int x);

int call_core_timer();
void call_sys_timer();

#endif

#endif  /*_SYS_H */
