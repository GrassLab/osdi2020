#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 2
#define CORE_TIMER    0   // syscal numbers
#define DAIF     1

#ifndef __ASSEMBLER__
int core_timer();
void daif();

int call_core_timer();
void call_daif();
#endif

#endif  /*_SYS_H */
