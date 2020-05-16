#ifndef THREAD_H
#define THREAD_H

#define STACK_SIZE 4096
#define MAX_TASK_SIZE 64

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
}__attribute__ ((aligned (8)));

typedef struct cpu_context cpu_context_t;


struct user_context {
    unsigned long sp_el0;   // user stack
    unsigned long elr_el1;  // user pc 
    unsigned long spsr_el1; // user cpu state
} __attribute__ ((aligned (8)));

typedef struct user_context user_context_t;

typedef struct trapframe {
	unsigned long regs[31];
} __attribute__ ((aligned (8))) Trapframe;

struct task {
    cpu_context_t cpu_context; 
    user_context_t user_context; 
    int task_id;
    int parent_id;
    long state;
    long counter;
    int mode;
    int rescheduled; 
    unsigned long trapframe;
};

typedef struct task task_t;

struct task_manager {
    task_t task_pool[MAX_TASK_SIZE];
    char kstack_pool[MAX_TASK_SIZE][STACK_SIZE];
    char ustack_pool[MAX_TASK_SIZE][STACK_SIZE];
    int task_num;
};

typedef struct task_manager task_manager_t;

enum {
    THREAD_RUNNABLE,
    THREAD_NOT_RUNNABLE,
    ZOMBIE,
};

enum {
    KERNEL_MODE,
    USER_MODE,
    INTERRUPT_MODE,
};

void privilege_task_create(unsigned long fn);
void create_idle_task();
void idle_task();
void init_task_manager();
void context_switch(struct task* next);
void cpu_switch_to(struct task*,struct task*);
struct task* get_current();
void ret_from_fork();
void fork_child_exit();
void schedule_tail(void);
void preempt_enable(void);
void do_exit();
void do_exec(void(*func)());
int do_fork();

#endif