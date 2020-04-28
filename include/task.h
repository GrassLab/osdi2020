#define RUN_IN_USER_MODE    1
#define IN_OUT_KERNEL       2
#define RUN_IN_KERNEL_MODE  3
#define IRQ_CONTEXT         4
#define CONTEXT_SWITCH      5


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
};

struct user_context {
    unsigned long sp_el0;   // user stack
    unsigned long spsr_el1; // user cpu state
    unsigned long elr_el1;  // user pc 
};

struct task {
    struct cpu_context cpu_context;
    struct user_context user_context;
    long state;
    long counter;
    long priority;  
    // long preempt_count;
    unsigned long task_id;
    int reschedule_flag;
};

struct task_manager {
    struct task task_pool[64];
    char kstack_pool[64][4096];
    char ustack_pool[64][4096];
    unsigned long queue_bitmap;
    unsigned int task_num;
    // struct task*(*current)();
};


void task_manager_init();
void privilege_task_create(void(*func)());
void context_switch(struct task* next);
struct task* get_current();
void set_current(struct task* task_struct);
void schedule();
void kernel_test();
void idle();
void user_test();
void do_exec(void(*func)());
void _setup_user_content(void(*func)());
void foo();

#define N 3

