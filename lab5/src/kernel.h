#ifndef KERNEL
#define KERNEL
#define QUEUE_SIZE 64

#define CORE0_TIMER_IRQ_CTRL (volatile unsigned int*)0x40000040

struct task_struct {
    unsigned long long x19;
    unsigned long long x20;
    unsigned long long x21;
    unsigned long long x22;
    unsigned long long x23;
    unsigned long long x24;
    unsigned long long x25;
    unsigned long long x26;
    unsigned long long x27;
    unsigned long long x28;
    unsigned long long x29;
    unsigned long long x30;
    char *kstack_top;
    char *ustack_top;

    int valid;
    int id;
    int status;
    int t_quantum;
};

struct run_queue {
    struct task_struct* queue[QUEUE_SIZE];
    unsigned int start;
    unsigned int end;
    int size;
};

//int reschedule = 0;

int kernel_init();

int privilege_task_create(void(*func)());

int do_exec(void(*func)());

int do_fork(unsigned long long sp);

int do_get_taskid();

void fork_context_copy(int parent_id, int child_id);

extern struct task_struct* get_current();

extern void el1_to_el0(void(*func)(), unsigned long long ustack);

void init();

void context_switch(struct task_struct* next);

void test();

void test_user();

void test_user2();

void test_user3();

void user();

void user2();

void user3();

int schedule();

void enable_sys_timer();

void check_resched_flag();

#endif
