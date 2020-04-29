



//extern struct task* get_current();
#define kernel_stack_size 10

typedef struct task{
    unsigned long long x19_x28[10];
    unsigned long long fp_lr[2]; //x29 x30
    unsigned long long ksp;
    unsigned long long sp_el0;
    unsigned long long elr_el1;
    unsigned long long spsr_el1;
    unsigned long long *trapframe;
    int id;
    int priority;
    char alive;
    char usage;
    char reschedule;
    char privilege;
    unsigned long long int start_coretime;
} task;

extern task task_pool[64];

typedef struct task_queue{
    int head;
    int tail;
    int now;
    struct task *taskq[64];
} task_queue;

//extern __attribute__((section(".userspace"))) char kstack_pool[64][4096];
//extern __attribute__((section(".userspace"))) char user_pool[64][4096];
    
//__attribute__ ((section (".userspace"))) char kstack_pool[64][4096];

//char *kstack_pool;
extern task_queue runqueue;
extern unsigned long long _global_coretimer;

int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void task_struct_init();
void runqueue_push(task *input);
void task_schedule();
task *get_current_task();
void toggle_privilege();
void do_exec(void(*func)());
void runqueue_del(int id);
