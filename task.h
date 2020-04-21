
typedef struct cpu_context_t {
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

typedef struct task_t{
/*
    int id;
    void (*func)();
*/
    cpu_context_t cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
}task_t;

#define TASK_IDLE 0
#define TASK_RUNNING 1
#define THREAD_SIZE 4096

#define INIT_TASK \
/*cpu_context*/ { {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */ 0,0,1, 0 \
}


static task_t *init_task = INIT_TASK;

task_t *task_pool[64] = {&(init_task),};

static int task_pool_len = 0;

static unsigned short kstack_pool[64][THREAD_SIZE] = {0};

static int runqueue[64];

static int runqueue_len = 0;


extern int get_current();
extern void set_current(int);
extern unsigned long ret_from_fork();

void schedule();

void init(){
    while(1){
        schedule();
    }
}

int privilege_task_create(unsigned long func, unsigned long arg){
    
    task_t *current = task_pool[get_current()];
    // preempt_disable();
    task_t *p;

    int task_id = task_pool_len;

    p = (task_t *) &kstack_pool[task_id][0];
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

    
    uart_send_int(p);
    uart_send('\n');
    //set_current(p);
    // preempt_enable();

    task_pool_len++;
    return task_id;

}

void task_init(){
    int task_id = privilege_task_create((unsigned long) init, "init");
    task_pool[task_id]->state = TASK_IDLE;
    set_current(task_id);
}

extern void switch_to(task_t*, task_t*);

void context_switch(int task_id){
    uart_send(':');
    uart_send_int((int)get_current());
    uart_send('>');
    uart_send_int((int)task_id);
    uart_send('\n');

    int prev_id = get_current();
    task_t *prev = &task_pool[get_current()];

    set_current(task_id);

    task_t *next = task_pool[task_id];

    switch_to(task_pool[prev_id], task_pool[task_id]);
}

static int schedule_index = 0;

// for core timer 1ms, 1 count
int schedule_cnt = 0;

void schedule(){
    uart_send_int(schedule_cnt);

    int task_id;
    task_t *p;
    while(1){
        task_id = (++schedule_index) % task_pool_len;
        p = task_pool[task_id];

        if(p && p->state == TASK_RUNNING){
            break;
        }
    }

    if(p == init_task){
        while(1){}
    }

    context_switch(task_id);

}

int check_reschedule(){
    // every 500ms, reschdule
    if(schedule_cnt > 500){
        schedule_cnt = 0;
        return 1;
    }
    else
        return 0;
}
