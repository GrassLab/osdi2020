#include "fork.h"
#include "sched.h"
#include "entry.h"
#include "mm.h"
#include "utils.h"

int __clone(unsigned long fn, unsigned long flags, 
            unsigned long child_stack /* reserve for further extension */) {
    preempt_disable();
    struct task_struct *t = (struct task_struct *) allocate_task_struct();
    CHECK(t)
    struct pt_regs *childregs = get_pt_regs(t);
    memzero((unsigned long)childregs, sizeof(struct pt_regs));
    memzero((unsigned long)&t->cpu_context, sizeof(struct cpu_context));

    if (flags & PF_KTHREAD) {
        t->cpu_context.x19 = fn;
    } else {
        // TODO: why need to copy this
        t->cpu_context.x19 = 0;
        struct pt_regs * cur_regs = get_pt_regs(current);
        *childregs = *cur_regs;

        // this implementation assume user stack allocate by kernel
        CHECK(!child_stack)
        child_stack = allocate_task_struct(); 
        childregs->sp = child_stack + PAGE_SIZE;
        t->stack = child_stack;

        // current->stack is pointer to user stack
        unsigned long user_stack_offset = cur_regs->regs[29] - (unsigned long)current->stack;
        childregs->regs[29] = user_stack_offset + child_stack;
        childregs->sp = childregs->regs[29]; // TODO: understand why
        childregs->regs[0] = 0;

        memcpy(childregs->regs[29], cur_regs->regs[29], PAGE_SIZE - user_stack_offset);
    }

    t->task_id = nr_tasks ++;
    t->priority = current->priority;
    t->counter = current->counter;
    t->state = TASK_RUNNING;
    t->preempt_count = 1;
    t->cpu_context.pc = (unsigned long)ret_from_fork;
    t->cpu_context.sp = (unsigned long)childregs;
    tasks[t->task_id] = t;
    preempt_enable();
    return t->task_id;
} 

int do_exec(unsigned long pc) {
    struct pt_regs *regs = get_pt_regs(current);
    memzero((unsigned long)regs, sizeof(*regs));
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;
    unsigned long stack = allocate_task_struct(); // allocate new user stack
    CHECK(stack)
    regs->sp = stack + PAGE_SIZE;
    current->stack = stack;
    return 0;
}

struct pt_regs* get_pt_regs(struct task_struct *t) {
    unsigned long p = (unsigned long)t + PAGE_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs*)p;
}
