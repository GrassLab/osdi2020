#define TASK_IDLE 0
#define TASK_RUNNING 1
#define THREAD_SIZE 4096

void schedule();
void context_switch(int task_id);
void task_init();
int privilege_task_create(unsigned long func, int usr);
void scs();
void ics();
void timer_tick();
void exec(unsigned long fun);
void do_exec();
void jmp_to_usr();
void ucs();
void do_ucs();

typedef struct cpu_context_t
{
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
} cpu_context_t;

typedef struct task_t
{
    cpu_context_t cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    int is_usr;
    int switchflag;
} task_t;

typedef struct user_task_context
{
    unsigned long spsr;
} user_task_context;

task_t *task_pool[64];