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

struct task {
    struct cpu_context cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    unsigned long task_id;
};

struct task_manager {
    struct task task_pool[64];
    char kstack_pool[64][4096];
    unsigned int task_num;
    // struct task*(*current)();
};

struct task_manager TaskManager;

void task_manager_init();
void privilege_task_create(void(*func)());
void context_switch(struct task* next);
struct task* get_current();
void set_current(struct task* task_struct);
void schedule();
void foo();
void idle();

#define N 10
