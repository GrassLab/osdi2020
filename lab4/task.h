



//extern struct task* get_current();


typedef struct task{
    unsigned long long x19_x28[10];
    unsigned long long fp_lr[2];
    unsigned long long sp;
    unsigned long long rip;
    int id;
    int priority;
    char alive;
    char usage;
    /*char reschedule;
    unsigned long long int start_coretime;*/
} task;
task task_pool[64];

struct task_queue{
    int head;
    int tail;
    int now;
    struct task *taskq[64];
} runqueue;

//__attribute__ ((section (".userspace")));
char kstack_pool[64][4096];
//__attribute__ ((section (".userspace")));


//unsigned long long _global_coretimer;

int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void task_struct_init();
void runqueue_push(task *input);
void task_schedule(unsigned long long rip);