



//extern struct task* get_current();


typedef struct task{
    unsigned long long x19_x28[10];
    unsigned long long fp_lr[2];
    unsigned long long ksp;
    unsigned long long rip;
    unsigned long long usp;
    unsigned long long x0_x18[19];
    unsigned long long x29_x30[2];
    int id;
    int priority;
    char alive;
    char usage;
    char reschedule;
    char privilege;
    unsigned long long spsr_el1;
    unsigned long long int start_coretime;
} task;

extern task task_pool[64];

typedef struct task_queue{
    int head;
    int tail;
    int now;
    struct task *taskq[64];
} task_queue;

    
//__attribute__ ((section (".userspace"))); char kstack_pool[64][4096];
//char *kstack_pool;
extern task_queue runqueue;

extern unsigned long long _global_coretimer;

int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void task_struct_init();
void runqueue_push(task *input);
void task_schedule(unsigned long long rip, unsigned long long sp, unsigned long long *oreg);
task *get_current_task();
void toggle_privilege();
