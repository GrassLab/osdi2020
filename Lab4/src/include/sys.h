#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 5
#define CORE_TIMER    0   // syscal numbers
#define DAIF     1
#define SYS_MALLOC 2
#define SYS_CLONE 3
#define SYS_EXIT 4

#ifndef __ASSEMBLER__
int core_timer();
void daif();
unsigned long sys_malloc();
int sys_clone(unsigned long stack);
void sys_exit();

int call_core_timer();
void call_daif();
unsigned long call_sys_malloc();
int call_sys_clone(unsigned long fn,unsigned arg,unsigned long stack);
void call_sys_exit();
#endif

#endif  /*_SYS_H */
