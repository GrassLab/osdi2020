#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define THREAD_CPU_CONTEXT 0 	// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__
#define NR_TASKS 64 
#define THREAD_SIZE  4096

#define TASK_RUNNING	0
#define TASK_ZOMBIE     1
#define TASK_WAIT       2

#define current get_current()

extern struct task_struct *task[NR_TASKS];

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
    unsigned long fp;
    unsigned long sp;
    unsigned long pc;
};

#define MAX_PROCESS_PAGES	16	
#define MAX_AREA                16

struct user_page {
	unsigned long phy_addr;
	unsigned long vir_addr;
};

//prot
#define PROT_NONE 0b000  // non-executable page frame for EL0
#define PROT_READ 0b100  // rwx bit represent
#define PROT_WRITE 0b110  
#define PROT_EXEC 0b101  

//flag
#define MAP_FIXED 0
#define MAP_ANONYMOUS 1
#define MAP_POPULATE 2

struct vm_area_struct{
	unsigned long vm_end;
	unsigned long vm_start;
	int vm_prot;
	int vm_flags;
};

struct mm_struct {
	unsigned long pgd;	
	int user_pages_count;
	struct user_page user_pages[MAX_PROCESS_PAGES];
	
	int kernel_pages_count;
	unsigned long kernel_pages[MAX_PROCESS_PAGES];
	
	int vm_area_count;
	struct vm_area_struct mmap[MAX_AREA];
};


struct signal_struct{
	int pending;
	int block;
};

struct task_struct{
	struct cpu_context cpu_context;
	struct signal_struct signal;
	struct mm_struct mm;

	int pid;
	long state;
	long priority;
	long counter;
	long preempt_lock;	
};

extern void switch_to(struct task_struct* prev, struct task_struct* next);
extern struct task_struct* get_current();
extern void init_idle_task(struct task_struct* task);

extern void schedule(void);
extern void context_switch(struct task_struct* next);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void schedule_tail(void);
extern void timer_tick();
extern void exit_process();

#define IDLE_TASK { {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0}, \
	{0,0,{{0}},0,{0},0,{{0}}}, \
	0,0,1,0,0}


#endif
#endif /*_SCHEDULER_H */
