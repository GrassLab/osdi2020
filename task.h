#include "irq.h"

#define PSR_MODE_EL0t 0x00000000
#define PSR_MODE_EL1t 0x00000004
#define PSR_MODE_EL1h 0x00000005
#define PSR_MODE_EL2t 0x00000008
#define PSR_MODE_EL2h 0x00000009
#define PSR_MODE_EL3t 0x0000000c
#define PSR_MODE_EL3h 0x0000000d

#define PF_KTHREAD 0x00000002
#define PAGE_SIZE 1024
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

/*
SP_EL0: The address of user mode’s stack pointer.

ELR_EL1: The program counter of user mode’s procedure.

SPSR_EL1: The CPU state of user mode.
*/
typedef struct user_context_t
{
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
} user_context_t;

typedef struct task_t
{
    cpu_context_t cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    unsigned long stack;
    unsigned long flags;
} task_t;

#define TASK_NUM 64

#define TASK_IDLE 0
#define TASK_RUNNING 1
#define THREAD_SIZE 4096

#define INIT_TASK                                     \
    /*cpu_context*/ {                                 \
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},      \
            /* state etc */ 0, 0, 1, 0, 0, PF_KTHREAD \
    }

static task_t *init_task = INIT_TASK;

task_t *task_pool[TASK_NUM] = {
    &(init_task),
};

static int task_pool_len = 0;

static unsigned short kstack_pool[TASK_NUM][THREAD_SIZE] = {0};

static unsigned short ustack_pool[TASK_NUM][THREAD_SIZE] = {0};

static int runqueue[TASK_NUM];

static int runqueue_len = 0;

extern int get_current();
extern void set_current(int);
extern unsigned long ret_from_fork();

void schedule();

void init()
{
    while (1)
    {
        schedule();
    }
}

int privilege_task_create(unsigned long func, unsigned long arg)
{

    task_t *current = task_pool[get_current()];
    // preempt_disable();
    task_t *p;

    int task_id = task_pool_len;

    p = (task_t *)&kstack_pool[task_id][0];
    task_pool[task_id] = p;
    if (!p)
        return -1;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule_tail

    p->cpu_context.x19 = func;
    p->cpu_context.x20 = arg;
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

    uart_send_int((unsigned long)p);
    uart_send('\n');
    //set_current(p);
    // preempt_enable();

    task_pool_len++;
    return task_id;
}

user_context_t *task_user_context(task_t *task)
{
    unsigned long p = (unsigned long)task + THREAD_SIZE - sizeof(user_context_t);
    return (user_context_t *)p;
}

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack)
{
    //preempt_disable();

    task_t *current = task_pool[get_current()];
    // preempt_disable();
    task_t *p;

    int task_id = task_pool_len;

    p = (task_t *)&kstack_pool[task_id][0];

    if (!p)
    {
        return -1;
    }

    user_context_t *childregs = task_user_context(p);
    memset((char *)childregs, 0, sizeof(user_context_t));
    memset((char *)&p->cpu_context, 0, sizeof(cpu_context_t));

    if (clone_flags & PF_KTHREAD)
    {
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    }
    else
    {
        user_context_t *cur_regs = task_user_context(current);
        *childregs = *cur_regs;
        childregs->regs[0] = 0;
        childregs->sp = stack + PAGE_SIZE;
        p->stack = stack;
    }
    p->flags = clone_flags;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule_tail

    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)childregs;

    int pid = task_pool_len++;
    task_pool[pid] = p;
    //preempt_enable();
    return pid;
}

int do_exec(unsigned long pc)
{

    user_context_t *regs = task_user_context(task_pool[get_current()]);
    memset(regs, 0, sizeof(*regs));
    regs->pc = pc;
    // regs->pstate = PSR_MODE_EL0t;
    unsigned long stack = (unsigned long)&ustack_pool[get_current()][0]; //allocate new user stack
    if (!stack)
    {
        return -1;
    }
    regs->sp = stack + PAGE_SIZE;
    // task_pool[get_current()]->stack = stack;
    return 0;
}

void task_init()
{
    int task_id = privilege_task_create((unsigned long)init, (unsigned long)"init");
    task_pool[task_id]->state = TASK_IDLE;
    set_current(task_id);
}

extern void switch_to(task_t *, task_t *);

void context_switch(int task_id)
{
    uart_send(':');
    uart_send_int((int)get_current());
    uart_send('>');
    uart_send_int((int)task_id);
    uart_send('\n');

    int prev_id = get_current();
    task_t *prev = task_pool[get_current()];

    set_current(task_id);

    task_t *next = task_pool[task_id];

    switch_to(task_pool[prev_id], task_pool[task_id]);
}

static int schedule_index = 0;

// for core timer 1ms, 1 count
int schedule_cnt = 0;

void schedule()
{
    int task_id;
    task_t *p;
    while (1)
    {
        task_id = (++schedule_index) % task_pool_len;
        p = task_pool[task_id];

        if (p && p->state == TASK_RUNNING)
        {
            break;
        }
    }

    if (p == init_task)
    {
        while (1)
        {
        }
    }

    context_switch(task_id);
}

int check_reschedule()
{
    // every 500ms, reschdule
    if (schedule_cnt > 500)
    {
        schedule_cnt = 0;
        return 1;
    }
    else
        return 0;
}

void timer_tick()
{
    if (check_reschedule() == 1)
    {
        enable_irq();
        schedule();
        disable_irq();
    }
}
