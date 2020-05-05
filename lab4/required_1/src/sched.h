#define THREAD_CPU_CONTEXT         0
#define NR_TASKS                   64

#define TASK_RUNNING               0
#define TASK_ZOMBIE                1

#define PF_KTHREAD                 0x00000002

#ifndef __ASSEMBLER__

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

struct task_struct {
    struct cpu_context cpu_context;
    unsigned long pid;
    unsigned long state;
    unsigned long counter;
    unsigned long priority;
    unsigned long preempt_count;
    // unsigned long need_reched;
    // unsigned long stack;
    // unsigned long flags;
};

extern struct task_struct *current;
struct task_struct *task[NR_TASKS];

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0 \
}


extern void cpu_switch_to(struct task_struct *, struct task_struct *);
extern struct task_struct *get_current();
extern void ret_from_fork();

void context_switch(struct task_struct *next);
void preempt_disable(void);
void preempt_enable(void);
struct task_struct *privilege_task_create(void (*func)(), unsigned long num);
void schedule_tail(void);

#endif