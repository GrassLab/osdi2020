#define MAX_TASK_NUM 64
#define STACK_SIZE 4096
#define current get_current()

struct cpu_context {
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
	unsigned long lr;
	unsigned long sp;
};

struct task {
	struct cpu_context cpu_context;
	int taskid;
	int counter;
	// long state;	
	// long priority;
	// long preempt_count;
};

extern struct task* get_current();
// extern struct task init;

struct task task_pool[MAX_TASK_NUM];
char kstack_pool[MAX_TASK_NUM][STACK_SIZE];
char ustack_pool[MAX_TASK_NUM][STACK_SIZE];

void init_task();
int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void idle_schedule();
void schedule();
void do_exec(void(*func)());
void do_fork();

struct run_queue{
    struct task* buf[MAX_TASK_NUM];
    int head;
    int tail;
} runqueue;










