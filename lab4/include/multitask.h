#ifndef _MULTITASK_H_
#define _MULTITASK_H_

/*
 * MEMORY MANAGEMENT
 */

#define PAGE_SHIFT              12    
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)
#define LOW_MEMORY              (2 * SECTION_SIZE)
#define HIGH_MEMORY             PBASE
#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

unsigned long get_free_page();
void free_page(unsigned long p);

/*
 * TASK MANAGEMENT
 */

#define TASK_RUNNING	0
#define TASK_IDLE		1
#define TASK_ZOMBIE		2

#define FLAG_KERNEL		0
#define FLAG_FORK		1

#define PSTATE_EL0t		0x00000000
#define PSTATE_EL1t		0x00000004
#define PSTATE_EL1h		0x00000005
#define PSTATE_EL2t		0x00000008
#define PSTATE_EL2h		0x00000009
#define PSTATE_EL3t		0x0000000c
#define PSTATE_EL3h		0x0000000d

#define THREAD_SIZE		4096
#define MAX_THREAD_NUM	16

struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;	// x29
	unsigned long sp;
	unsigned long pc;	// x30
};

struct task_struct {
	struct cpu_context _cpu_context;
	long state;
	long counter;
	long priority;
	long preempt_count;
	long stack;
	long flags;
};



struct user_context {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};

void init();
int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long free_old_stack);
int do_fork();
int do_exec(unsigned long address);
void exit_process(int pid);
void context_switch(int pid);
void schedule();
int check_reschedule();
void timer_tick();
void schedule_cnt_add(int num);

extern void switch_to(struct task_struct *, struct task_struct *);
extern int get_current();
extern int set_current(int tid);
extern unsigned long ret_from_fork();


#endif
