#include "fork.h"
#include "sched.h"
#include "entry.h"
#include "mm.h"

int _fork() {
    preempt_disable();

    if (current->fork_flag == 1) {
        current->fork_flag = 0;
        get_pt_regs(current)->regs[0] = 0;
        preempt_enable();
        return 0;
    }

    struct task_struct *t;
    
    t = (struct task_struct *) allocate_a_page();
    if (!t) {
        return -1;
    }

    memcpy((unsigned long)t, (unsigned long)current, PAGE_SIZE);

    struct pt_regs *childregs = get_pt_regs(t);
    struct pt_regs *cur_regs = get_pt_regs(current);

    *childregs = *cur_regs;
    childregs->regs[0] = 0;

    childregs->sp = (unsigned long)t + ((unsigned long)cur_regs - (unsigned long)current);
    childregs->regs[29] = childregs->sp;

    *t = *current;

    t->cpu_context.sp = (unsigned long)childregs;
    t->cpu_context.pc = (unsigned long)ret_from_fork_user;
    // childregs->regs[19] = 0;
    t->task_id = num_tasks;
    t->fork_flag = 1;

    int pid = num_tasks ++;
    tasks[pid] = t;

    preempt_enable();
    uart_puts("pid: ");
    uart_send_ulong(pid);
    uart_send('\n');
    return pid;
}

int _task_create(enum privilege_t privilege, unsigned long func_ptr) {
    preempt_disable(); 
    struct task_struct *t;

    /* task struct place at top of the allocate memory */
    t = (struct task_struct *) allocate_a_page(); 
    if (!t) {
        return -1;
    }
    t->task_id = num_tasks;
    t->priority = 5;
    t->state = TASK_RUNNING;
    t->counter = 5;
    t->preempt_count = 1; 
    
    t->cpu_context.x19 = func_ptr;
    if (privilege == KERNEL_MODE) {
        t->cpu_context.pc = (unsigned long)ret_from_fork;
    } else {
        t->cpu_context.pc = (unsigned long)ret_from_fork_user;
    }
    // t->cpu_context.sp = (unsigned long)t + PAGE_SIZE;
    t->cpu_context.sp = (unsigned long)t + PAGE_SIZE - sizeof(struct pt_regs);

    /* TODO: A simple implementation, 
    which will cause error if create more than 64 task */
    int pid = num_tasks ++;
    tasks[pid] = t;
    preempt_enable();
    return 0;
}

int move_to_user_mode(unsigned long pc) {
    struct pt_regs *regs = get_pt_regs(current); 
    memzero((unsigned long)regs, sizeof(struct pt_regs)); 
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t; // TODO: ??

    unsigned long page = allocate_a_page(); // allocate a page for user stack
    if (!page) {
        return -1;
    }
    regs->sp = page + PAGE_SIZE;
    // current->stack = page; // Start to use user stack, TODO: why set current
    return 0;
}

int do_exec(unsigned long pc) {
    move_to_user_mode(pc);
}

struct pt_regs* get_pt_regs(struct task_struct *t) {
    unsigned long p = (unsigned long)t + PAGE_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs*)p;
}
