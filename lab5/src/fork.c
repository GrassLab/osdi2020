#include "mm.h"
#include "sched.h"
#include "fork.h"
#include "entry.h"
#include "utils.h"

int privilege_task_create(unsigned long clone_flags, unsigned long fn, unsigned long arg){
    preempt_disable();

    struct task *p;
    p = (struct task*) get_free_page();
    if (!p)
        return 1;

    struct pt_regs *childregs = task_pt_regs(p);
    memzero((unsigned long)childregs, sizeof(struct pt_regs));
    memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));

    if (clone_flags & PF_KTHREAD) {
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    } else {
        struct pt_regs * cur_regs = task_pt_regs(current);
        *childregs = *cur_regs;
        childregs->regs[0] = 0; //return value pid
        int copy_byte = (current->stack + PAGE_SIZE) - cur_regs->regs[29];
        unsigned long stack = get_free_page();
        if(!stack){
            return -1;
        }
        childregs->regs[29] = (stack + PAGE_SIZE) - copy_byte;
        childregs->sp = childregs->regs[29];
        p->stack = stack;
		fork_memcpy((void *)childregs->regs[29],(void *)cur_regs->regs[29],copy_byte);
        p->cpu_context.x19 = 0;
    }    
    p->flags = clone_flags;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule done

    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)childregs;
    p->need_resched = 0;
    int pid = nr_tasks++;
    exist++;
    p->id = pid;
    task_pool[pid] = p;

    uart_puts("Task_create:");
    uart_hex(p->id);
    uart_puts("\n");

    preempt_enable();
    return pid;
}



int do_exec(unsigned long pc)
{
    struct pt_regs *regs = task_pt_regs(current);
    memzero((unsigned long)regs, sizeof(*regs));
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;
    unsigned long stack = get_free_page(); //allocate new user stack
    if (!stack) {
        return -1;
    }
    regs->sp = stack + PAGE_SIZE;
    current->stack = stack;
    return 0;
}

struct pt_regs * task_pt_regs(struct task *tsk){
    unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs *)p;
}
