#define INIT                0
#define RUN_IN_USER_MODE    1
#define IN_OUT_KERNEL       2
#define RUN_IN_KERNEL_MODE  3
#define IRQ_CONTEXT         4
#define CONTEXT_SWITCH      5
#define ZOMBIE              6
#define EXC_CONTEXT         7


// the cpu_context's order must be the same as switch_to
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
} __attribute__ ((aligned (8)));

struct user_context {
    unsigned long sp_el0;   // user stack
    unsigned long elr_el1;  // user pc 
    unsigned long spsr_el1; // user cpu state
} __attribute__ ((aligned (8)));

struct task {
    struct cpu_context cpu_context;
    struct user_context user_context;
    long state;
    long counter;
    long priority;  
    // long preempt_count;
    unsigned long task_id;
    unsigned long parent_id;
    int reschedule_flag;
    unsigned long trapframe; // only for syscall, eg. fork
    // struct trapframe_regs trapframe_regs;
};

struct task_manager {
    struct task task_pool[64];
    char kstack_pool_prevent[4096];
    char kstack_pool[64][4096];
    char ustack_pool[64][4096];
    unsigned long queue_bitmap;
    unsigned long zombie_num;
    unsigned int task_num;
    // struct task*(*current)();
};


struct trapframe_regs {
	unsigned long regs[31];
	unsigned long sp_el0; // sp
	unsigned long elr_el1; // pc
	unsigned long spsr_el1; // pstate
} __attribute__ ((aligned (8)));

void task_manager_init(void(*func)());
int privilege_task_create(void(*func)(), int fork_flag);
void context_switch(struct task* next);
struct task* get_current();
void set_current(struct task* task_struct);
void schedule();
void kernel_test();
void idle();
void user_test();
void foo();
void _setup_user_content(void(*func)());

void do_exec(void(*func)());
int fork();
int exit(int status);
void zombie_reaper();

void final_test_foo();
void final_test(); 
void final_user_test();
void final_idle();

#define N 3
#define CNT 0x2000
