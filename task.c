#include "irq.h"
#include "task.h"
#include "string.h"
#include "uart.h"
#include "syscall.h"
#include "mm.h"

#include "debug.h"

task_t *task_pool[TASK_NUM] = {0};

static int task_pool_len = 0;

// unsigned long kstack_pool[TASK_NUM][THREAD_SIZE] = {0};

// unsigned long ustack_pool[TASK_NUM][THREAD_SIZE] = {0};

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

user_context_t *task_user_context(task_t *task)
{
    unsigned long p = (unsigned long)task + THREAD_SIZE - sizeof(user_context_t);
    return (user_context_t *)p;
}

void print_cpu_context(cpu_context_t *c)
{
    printf("============\n\r");
    printf("x19: %x\n\r", c->x19);
    printf("x20: %x\n\r", c->x20);
    printf("x21: %x\n\r", c->x21);
    printf("x22: %x\n\r", c->x22);
    printf("x23: %x\n\r", c->x23);
    printf("x24: %x\n\r", c->x24);
    printf("x25: %x\n\r", c->x25);
    printf("x26: %x\n\r", c->x26);
    printf("x27: %x\n\r", c->x27);
    printf("x28: %x\n\r", c->x28);
    printf("fp: %x\n\r", c->fp);
    printf("sp: %x\n\r", c->sp);
    printf("pc: %x\n\r", c->pc);
    printf("============\n\r");
}

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack)
{
    //preempt_disable();

    task_t *current = task_pool[get_current()];
    task_t *p;

    int task_id = task_pool_len;
    DEBUG_LOG_TASK(("c:%d, %d, %d;\n\r", get_current(), task_id, clone_flags));

    //p = (task_t *)&kstack_pool[task_id][0];
    p = (task_t *)get_free_page();

    // test visual memory map to physical memory correctly
    /*
    // check visual address
    uart_send_hex(sizeof(mem_page_t));
    uart_send('\n');
    uart_send_hex((unsigned long)p >> 32);
    uart_send_hex((unsigned long)p);
    // reset memory page default value
    ((unsigned long *)p)[0] = task_id;
    uart_send('\n');
    // check value in virtual address
    uart_send_int(*(unsigned long *)p);
    uart_send(' ');
    // check value in physical address
    uart_send_int(*(unsigned long *)((unsigned long)p - 0xffff000000000000 + mem_map_pa));
    uart_send(' ');
    // check value in virtual address
    uart_send_int(*(unsigned long *)(va_to_pa((unsigned long)p)));
    uart_send(' ');
    // check va to pa correctly
    uart_send_hex(va_to_pa((unsigned long)p));
    uart_send(' ');
    // check pa to pfn
    uart_send_int((pa_to_pfn(va_to_pa((unsigned long)p))));
    uart_send('\n');
    */

    memset((unsigned short *)p, 0, PAGE_SIZE);

    user_context_t *childregs = task_user_context(p);
    memset((unsigned short *)childregs, 0, sizeof(user_context_t));
    memset((unsigned short *)&p->cpu_context, 0, sizeof(cpu_context_t));

/*
    if (stack != 0)
        free_page(pa_to_pfn(stack));
        */
    stack = get_free_page();
    memset((unsigned short *)stack, 0, PAGE_SIZE);

    if (clone_flags & PF_KTHREAD)
    {
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;

        p->cpu_context.sp = (unsigned long)childregs;
    }
    else if (clone_flags & PF_FORK)
    {
        memcpy((unsigned short *)p, (unsigned short *)current, PAGE_SIZE);
        memcpy((unsigned short *)stack, (unsigned short *)current->stack, PAGE_SIZE);
        memset((unsigned short *)&p->cpu_context, 0, sizeof(cpu_context_t));

        // print_cpu_context(&current->cpu_context);

        /*** try to fork all kernel thread ***/
        //*p = *current;

        //printf("%x\n\r", childregs->sp);
        //p->cpu_context.sp = childregs->sp;
        //p->cpu_context.pc = p->cpu_context.pc + 32;
        p->stack = stack;

        /*
        printf("==current kstack, sp: %x, stack %x==\n\r", (unsigned long)current->cpu_context.sp, (unsigned long)current);
        printf("%x", (unsigned long)task_user_context(current));
        printf("==current ustack, sp: %x, stack %x==\n\r", (unsigned long)task_user_context(current)->sp, (unsigned long)current->stack);
        printf("==pc: %x, pc %x==\n\r", (unsigned long)task_user_context(current)->pc, childregs->pc);


        printf("%d", PAGE_SIZE);
        printf("task off %d, u off %d\n\r", kstack_offset, ustack_offset);
        */

        p->cpu_context.sp = 0xffff000000000000 + PAGE_SIZE;

       // p->cpu_context.x19 = current->cpu_context.x19;

        childregs->sp = 0xffff000000000000 + PAGE_SIZE - sizeof(user_context_t);

        // create new pgd for user space memory map
        unsigned long pgd = get_free_page();
        unsigned long pud = get_free_page();
        unsigned long pmd = get_free_page();
        unsigned long ptd = get_free_page();
        ((unsigned long *)pgd)[0] = pud | // to PUD
                                    PT_TABLE | PT_AF | PT_USER;
        ((unsigned long *)pud)[0] = pmd | // to PmD
                                    PT_TABLE | PT_AF | PT_USER;
        ((unsigned long *)pmd)[0] = ptd | // to PtD
                                    PT_TABLE | PT_AF | PT_USER;
        ((unsigned long *)ptd)[0] = stack | // memory
                                    PT_PAGE | PT_AF | PT_USER;

        current->pgd = pgd;
        current->pud = pud;
        current->pmd = pmd;
        current->ptd = ptd;
    }
    else
    {
        // stack = p;
        user_context_t *cur_regs = task_user_context(current);
        *childregs = *cur_regs;
        childregs->regs[0] = 0;
        childregs->sp = stack + PAGE_SIZE;
        p->stack = stack;

        p->cpu_context.sp = (unsigned long)childregs;
    }
    p->flags = clone_flags;
    //p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule_tail
    p->signal_source = 0;

    if (clone_flags & PF_FORK)
    {
        // asm volatile("mov x0, %0;" :: "r"(current->cpu_context.x19));
        p->cpu_context.pc = (unsigned long)ret_from_fork;
    }
    else
    {
        p->cpu_context.pc = (unsigned long)ret_from_fork;
    }

    if (clone_flags & PF_FORK)
    {
        //p->cpu_context.pc += 64;
    }
    //printf("sp: %x, %x\n\r", p->cpu_context.sp, current->cpu_context.sp);
    //print_cpu_context(&p->cpu_context);

    task_pool_len++;
    task_pool[task_id] = p;
    //preempt_enable();
    return task_id;
}

int privilege_task_create(unsigned long func, unsigned long arg)
{
    return copy_process(PF_KTHREAD, func, arg, 0);
}

int _do_exec(unsigned long start, unsigned long size)
{
    task_t *current = task_pool[get_current()];
    user_context_t *regs = task_user_context(current);
    memset(regs, 0, sizeof(user_context_t));
    regs->pc = 0xffff000000000000;
    regs->pstate = PSR_MODE_EL0t;
    current->cpu_context.pc = regs->pc;

    // pa
    unsigned long stack = get_free_page();
    memset((unsigned char *)stack, 0, PAGE_SIZE);

    //uart_send_int(size);
    memcpy((unsigned char *)stack, (unsigned char *)start, size);

    // use visual address replace stack(physical address)
    regs->sp = 0xffff000000000000 + PAGE_SIZE;
    current->stack = 0xffff000000000000;



    // create new pgd for user space memory map
    unsigned long pgd = get_free_page();
    unsigned long pud = get_free_page();
    unsigned long pmd = get_free_page();
    unsigned long ptd = get_free_page();
    ((unsigned long *)pgd)[0] = pud | // to PUD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)pud)[0] = pmd | // to PUD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)pmd)[0] = ptd | // to PUD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)ptd)[0] = stack | // to PUD
                                PT_PAGE | PT_AF | PT_USER;

    set_pgd(pgd);

    // check user space set in 0xffff000000000000
    // it will break loaded program
    /*
    uart_send_hex((unsigned long)((unsigned char *)stack)[0]);
    ((unsigned long *)(0xffff000000000000))[0] = 0xff;
    */

    // test page fault
    // ((unsigned char *)(0xffff000000000000))[PAGE_SIZE] = 0xffff;

    current->pgd = pgd;
    current->pud = pud;
    current->pmd = pmd;
    current->ptd = ptd;

    // asm volatile("mov x30, %0" ::"r"(0xffff000000000000));

    //((void (*)(void))(0xffff000000000000))();
    return 0;
}

int do_exec(unsigned long pc)
{
    task_t *current = task_pool[get_current()];
    user_context_t *regs = task_user_context(current);
    memset(regs, 0, sizeof(user_context_t));
    regs->pc = 0xffff000000000000;
    regs->pstate = PSR_MODE_EL0t;
    current->cpu_context.pc = regs->pc;
    unsigned long stack = get_free_page();
    memset((unsigned char *)stack, 0, PAGE_SIZE);

    memcpy((unsigned char *)stack, (unsigned char *)pc, PAGE_SIZE);

    // use visual address replace stack(physical address)
    regs->sp = 0xffff000000000000 + PAGE_SIZE;
    current->stack = 0xffff000000000000;

    // create new pgd for user space memory map
    unsigned long pgd = get_free_page();
    unsigned long pud = get_free_page();
    unsigned long pmd = get_free_page();
    unsigned long ptd = get_free_page();
    ((unsigned long *)pgd)[0] = pud | // to PUD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)pud)[0] = pmd | // to PmD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)pmd)[0] = ptd | // to PtD
                                PT_TABLE | PT_AF | PT_USER;
    ((unsigned long *)ptd)[0] = stack | // memory
                                PT_PAGE | PT_AF | PT_USER;

    set_pgd(pgd);
    // test page fault
    // ((unsigned char *)(0xffff000000000000))[PAGE_SIZE] = 0xffff;

    current->pgd = pgd;
    current->pud = pud;
    current->pmd = pmd;
    current->ptd = ptd;
    return 0;
}

void task_init()
{
    set_current(0);
    int task_id = copy_process(PF_KTHREAD, (unsigned long)init, 0, 0);
    task_pool[task_id]->state = TASK_IDLE;
}

void set_pgd(unsigned long pgd)
{
    // upper half, user space
    asm volatile("dsb ish");
    asm volatile("isb");
    asm volatile("msr ttbr1_el1, %0"
                 :: "r"((unsigned long)(pgd)));

    asm volatile("tlbi vmalle1is");
    asm volatile("dsb ish");
    asm volatile("isb");
}

void context_switch(int task_id)
{
    extern volatile unsigned char _end;
    int current_el = get_current();
    DEBUG_LOG_TASK((":%d>%d\n\r", current_el, task_id));

    int prev_id = get_current();
    task_t *prev = task_pool[get_current()];
    set_current(task_id);

    task_t *next = task_pool[task_id];

    set_pgd(next->pgd);

    switch_to(prev, next, next->signal_source);
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
            //DEBUG_LOG_TASK(("\nTASK: %d, %d\n", task_id, p->state));
        }
    }

    context_switch(task_id);
}

int check_reschedule()
{
    // every 5ms, reschdule
    if (schedule_cnt > 5)
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

void exit_process(int task_id)
{
    task_t *current = task_pool[task_id];
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
        free_page(current->stack_pfn);
    }
    if (current->pgd)
        free_page(pa_to_pfn(current->pgd));
    if (current->pud)
        free_page(pa_to_pfn(current->pud));
    if (current->pmd)
        free_page(pa_to_pfn(current->pmd));
    if (current->ptd)
        free_page(pa_to_pfn(current->ptd));
    //preempt_enable();
    schedule();
}

int do_fork()
{
    DEBUG_LOG_TASK(("fork current taskid[%d]\n\r", get_current()));
    // unsigned long stack = (unsigned long)&kstack_pool[get_current()][0];
    unsigned long stack = get_free_page();
    memset((unsigned long *)stack, 0, PAGE_SIZE);

    //return copy_process(0, 0, 0, stack);
    return copy_process(PF_FORK, 0, 0, stack);
}

task_t *task(int task_id)
{
    return task_pool[task_id];
}

unsigned long task_signal_source()
{
    return task_pool[get_current()]->signal_source;
}
