#include "irq.h"
#include "task.h"
#include "string.h"
#include "uart.h"

#include "debug.h"

task_t *task_pool[TASK_NUM] = {0};

static int task_pool_len = 0;

unsigned long kstack_pool[TASK_NUM][THREAD_SIZE] = {0};

unsigned long ustack_pool[TASK_NUM][THREAD_SIZE] = {0};

static int runqueue[TASK_NUM] = {0};

static int runqueue_len = 0;
static int schedule_index = 0;

// for core timer 1ms, 1 count
static int schedule_cnt = 0;

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
    task_t *p;

    int task_id = task_pool_len;

    p = (task_t *)&kstack_pool[task_id][0];
    memset((unsigned long *)p, 0, THREAD_SIZE);

    user_context_t *childregs = task_user_context(p);
    memset((unsigned long *)childregs, 0, sizeof(user_context_t));
    memset((unsigned long *)&p->cpu_context, 0, sizeof(cpu_context_t));

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
        p->state = TASK_RUNNING;
    }
    p->flags = clone_flags;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule_tail

    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)childregs;

    task_pool_len++;
    task_pool[task_id] = p;
    //preempt_enable();
    return task_id;
}

int do_exec(unsigned long pc)
{

    user_context_t *regs = task_user_context(task_pool[get_current()]);
    memset(regs, 0, sizeof(*regs));
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;
    unsigned long stack = (unsigned long)&ustack_pool[get_current()][0]; //allocate new user stack
    memset((unsigned long *)stack, 0, THREAD_SIZE);

    regs->sp = stack + PAGE_SIZE;
    // task_pool[get_current()]->stack = stack;
    return 0;
}

void task_init()
{
    int task_id = copy_process(PF_KTHREAD, (unsigned long)init, (unsigned long)"init", 0);
    task_pool[task_id]->state = TASK_IDLE;
    set_current(task_id);
}

void context_switch(int task_id)
{
    int current_el = get_current();
    DEBUG_LOG_TASK((":%d>%d\n\r", current_el, task_id));

    int prev_id = get_current();
    task_t *prev = task_pool[get_current()];

    set_current(task_id);

    task_t *next = task_pool[task_id];

    switch_to(prev, next);
}

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
        else
        {
            DEBUG_LOG_TASK(("\nTASK: %d, %d\n", task_id, p->state));
        }
    }

    context_switch(task_id);
}

int check_reschedule()
{
    // every 500ms, reschdule
    if (schedule_cnt > 30)
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

void schedule_cnt_add(int n)
{
    schedule_cnt += n;
}

void exit_process()
{
    task_t *current = task_pool[get_current()];
    // preempt_disable();
    for (int i = 0; i < TASK_NUM; i++)
    {
        if (task_pool[i] == current)
        {
            task_pool[i]->state = TASK_ZOMBIE;
            break;
        }
    }
    if (current->stack)
    {
        //free_page(current->stack);
    }
    //preempt_enable();
    schedule();
}

int do_fork()
{
    unsigned long stack = (unsigned long)&kstack_pool[7][0];
    memset((unsigned long *)stack, 0, THREAD_SIZE);

    return copy_process(0, 0, 0, stack);
}